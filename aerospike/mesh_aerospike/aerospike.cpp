#include <gmsh.h>
#include <cmath>
#include <algorithm>

int main(int argc, char **argv) {
    gmsh::initialize(argc, argv);
    gmsh::model::add("aerospike");

    // Parameters
    double ls = 1e-1;
    const double angle_deg = 1.0;  // angle of the wedge
    const double angle_rad = angle_deg * M_PI / 180.0;
    const double L_throat = 1.0;
    const double L_exit = 3.0;
    const double H_throat = 0.7;
    const double H_exit = 0.1;
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
        pointTags.push_back(gmsh::model::geo::addPoint(p.first, p.second, 0.0, ls));
    }

    // Connect points with lines
    std::vector<int> lineTags;
    for (size_t i = 0; i < pointTags.size(); i++) {
        int next = (i + 1) % pointTags.size();
        lineTags.push_back(gmsh::model::geo::addLine(pointTags[i], pointTags[next]));
    }

    int curveLoop = gmsh::model::geo::addCurveLoop(lineTags);
    int surface = gmsh::model::geo::addPlaneSurface({curveLoop});

    // Угол поворота (в радианах)
    double theta = -angle_rad / 2; // 30 градусов

    // Вращаем поверхность вокруг оси X, проходящей через начало координат (0, 0, 0)
    gmsh::model::geo::rotate(
        {{2, surface}},  // Объект для вращения
        0.0, 0.0, 0.0,           // Центр вращения (ось проходит через эту точку)
        1.0, 0.0, 0.0,           // Направление оси вращения (ось X)
        theta                   // Угол вращения
    );

    // Rotate the profile
    std::vector<std::pair<int, int>> extruded;

    // Rotation (+1°)
    gmsh::model::geo::revolve(
        {{2, surface}},
        0.0, 0.0, 0.0,   // Axis origin
        1.0, 0.0, 0.0,    // Axis direction (X-axis)
        angle_rad,        // Positive angle
        extruded
    );

    gmsh::model::geo::synchronize();

    // Combine the two rotated volumes (if needed)
    // Note: GMSH may create separate volumes; merge them if necessary.
    // For OpenFOAM, you can keep them separate but assign the same physical group.

    // Assign physical groups
    int wedge1 = extruded[0].second;          // Wedge1
    int wedge2 = extruded[1].second;          // Wedge2

    int inlet = extruded[6].second;  // Inlet

    int wall1 = extruded[2].second;       
    int wall2 = extruded[3].second; // Wall

    int outlet1 = extruded[4].second;      
    int outlet2 = extruded[5].second;     // Outlet 

    // Group surfaces
    gmsh::model::addPhysicalGroup(2, {inlet}, 1, "inlet");
    gmsh::model::addPhysicalGroup(2, {outlet1, outlet2}, 2, "outlet");
    gmsh::model::addPhysicalGroup(2, {wall1, wall2}, 3, "walls");
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
    gmsh::write("aerospike.msh");

    // Optional: Visualize
    gmsh::fltk::run();
    gmsh::finalize();
    return 0;
}