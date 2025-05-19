#include <gmsh.h>
#include <cmath>
#include <algorithm>

int main(int argc, char **argv) {
    gmsh::initialize(argc, argv);
    gmsh::model::add("laval");

    // Parameters
    const double angle_deg = 1.0;  // Half-angle of the wedge (e.g., 1° → total 2° sector)
    const double angle_rad = angle_deg * M_PI / 180.0;
    const double L_throat = 1.0;
    const double L_exit = 3.0;
    const double H_throat = 0.3;
    const double H_exit = 1.0;
    const double L_box = 2.0;

    // Create 2D profile (nozzle contour)
    std::vector<std::pair<double, double>> points = {
        {0.0, 0.5},          // Inlet (top)
        {L_throat, H_throat}, // Throat
        {L_exit, H_exit},     // Nozzle exit
        {L_exit + L_box, H_exit}, // Outlet box (top)
        {L_exit + L_box, 0.0},    // Outlet box (bottom)
        {L_exit, 0.0},        // Nozzle exit (bottom)
        {L_throat, 0.0},      // Throat (bottom)
        {0.0, 0.0}           // Inlet (bottom)
    };

    // Add points to GMSH
    std::vector<int> pointTags;
    for (const auto &p : points) {
        pointTags.push_back(gmsh::model::geo::addPoint(p.first, p.second, 0.0));
    }

    // Connect points with lines
    std::vector<int> lineTags;
    for (size_t i = 0; i < pointTags.size(); i++) {
        int next = (i + 1) % pointTags.size();
        lineTags.push_back(gmsh::model::geo::addLine(pointTags[i], pointTags[next]));
    }

    std::reverse(lineTags.begin(), lineTags.end()); // Reverse normal direction
    int curveLoop = gmsh::model::geo::addCurveLoop(lineTags);
    int surface = gmsh::model::geo::addPlaneSurface({curveLoop});

    // Rotate the profile in BOTH directions (±angle_deg)
    std::vector<std::pair<int, int>> extruded_pos, extruded_neg;

    // Positive rotation (+1°)
    gmsh::model::geo::revolve(
        {{2, surface}},
        0.0, 0.0, 0.0,   // Axis origin
        1.0, 0.0, 0.0,    // Axis direction (X-axis)
        angle_rad,        // Positive angle
        extruded_pos
    );

    // Negative rotation (-1°)
    gmsh::model::geo::revolve(
        {{2, surface}},
        0.0, 0.0, 0.0,   // Axis origin
        1.0, 0.0, 0.0,    // Axis direction (X-axis)
        -angle_rad,       // Negative angle
        extruded_neg
    );

    gmsh::model::geo::synchronize();

    // Combine the two rotated volumes (if needed)
    // Note: GMSH may create separate volumes; merge them if necessary.
    // For OpenFOAM, you can keep them separate but assign the same physical group.

    // Assign physical groups
    int inlet_surf_pos = extruded_pos[6].second;  // Inlet (+1° side)
    int inlet_surf_neg = extruded_neg[6].second;  // Inlet (-1° side)
    int wedge1 = extruded_pos[0].second;          // Wedge1 (+1° face)
    int wedge2 = extruded_neg[0].second;          // Wedge2 (-1° face)
    int wall1_pos = extruded_pos[2].second;       // Wall (+1° side)
    int wall2_pos = extruded_pos[3].second;
    int wall1_neg = extruded_neg[2].second;       // Wall (-1° side)
    int wall2_neg = extruded_neg[3].second;
    int outlet_pos = extruded_pos[4].second;      // Outlet (+1° side)
    int outlet_neg = extruded_neg[4].second;     // Outlet (-1° side)

    // Group surfaces
    gmsh::model::addPhysicalGroup(2, {inlet_surf_pos, inlet_surf_neg}, 1, "inlet");
    gmsh::model::addPhysicalGroup(2, {outlet_pos, outlet_neg}, 2, "outlet");
    gmsh::model::addPhysicalGroup(2, {wall1_pos, wall2_pos, wall1_neg, wall2_neg}, 3, "walls");
    gmsh::model::addPhysicalGroup(2, {wedge1}, 4, "wedge1");
    gmsh::model::addPhysicalGroup(2, {wedge2}, 5, "wedge2");

    // Volume
    std::vector<std::pair<int, int>> volumes;
    gmsh::model::getEntities(volumes, 3);
    if (volumes.empty()) {
        gmsh::logger::write("No 3D elements generated!", "error");
        return 1;
    }
    gmsh::model::addPhysicalGroup(3, {volumes[0].second, volumes[1].second}, 1, "fluid");

    // Generate mesh
    gmsh::model::geo::synchronize();
    gmsh::option::setNumber("Mesh.MeshSizeMin", 0.01);
    gmsh::option::setNumber("Mesh.MeshSizeMax", 0.03);
    gmsh::model::mesh::generate(3);

    // Save mesh (legacy format for OpenFOAM)
    gmsh::option::setNumber("Mesh.MshFileVersion", 2.2);
    gmsh::write("laval.msh");

    // Optional: Visualize
    gmsh::fltk::run();
    gmsh::finalize();
    return 0;
}