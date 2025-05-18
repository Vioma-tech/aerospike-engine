#include <gmsh.h>
#include <cmath>
#include <algorithm>

int main(int argc, char **argv) {
    gmsh::initialize(argc, argv);
    gmsh::model::add("laval");
    // gmsh::option::setNumber("Mesh.Normals", 1);  // Включить нормали
    // gmsh::option::setNumber("Mesh.ElementOrder", 1);  // Force linear elements
    // gmsh::option::setNumber("Mesh.SecondOrderLinear", 1);  // Improve linear accuracy

    // Параметры
    const double angle_deg = 1.0;  // Угол наклона сопла
    const double angle_rad = angle_deg * M_PI / 180.0;
    const double L_throat = 1.0;    // Длина горловины
    const double L_exit = 3.0;     // Длина расширяющейся части
    const double H_throat = 0.3;   // Высота горловины
    const double H_exit = 1.0;     // Высота на выходе
    const double L_box = 2.0;      // Длина выходного отсека

    // Создаём 2D-профиль сопла (контур)
    std::vector<std::pair<double, double>> points = {
        {0.0, 0.5},         // Вход (верх)
        {L_throat, H_throat}, // Горловина
        {L_exit, H_exit},     // Выход сопла
        {L_exit + L_box, H_exit}, // Конец отсека (верх)
        {L_exit + L_box, 0.0},    // Конец отсека (низ)
        {L_exit, 0.0},       // Выход сопла (низ)
        {L_throat, 0.0},      // Горловина (низ)
        {0.0, 0.0}          // Вход (низ)
    };

    // Добавляем точки в GMSH
    std::vector<int> pointTags;
    for (const auto &p : points) {
        pointTags.push_back(gmsh::model::geo::addPoint(p.first, p.second, 0.0));
    }

    // Соединяем точки линиями
    std::vector<int> lineTags;
    for (size_t i = 0; i < pointTags.size(); i++) {
        int next = (i + 1) % pointTags.size();
        lineTags.push_back(gmsh::model::geo::addLine(pointTags[i], pointTags[next]));
    }

    std::reverse(lineTags.begin(), lineTags.end()); // нормаль в другую сторону
    // Замыкаем контур и создаём поверхность
    int curveLoop = gmsh::model::geo::addCurveLoop(lineTags);
    int surface = gmsh::model::geo::addPlaneSurface({curveLoop});

    // Вращаем профиль вокруг оси X с поворотом на 5°
    std::vector<std::pair<int, int>> extruded;
    gmsh::model::geo::revolve(
        {{2, surface}},  // Поверхность для вращения
        0.0, 0.0, 0.0,   // Точка начала оси вращения
        1.0, 0.0, 0.0,   // Направление оси (X)
        angle_rad,        // Угол поворота (5° в радианах)
        extruded
    );


    gmsh::model::geo::synchronize();
    gmsh::model::mesh::generate(3);  // Explicitly generate 3D mesh

    // Assign physical groups using extruded entities
    int inlet_surf = extruded[6].second;  // (inlet)
    int outlet_surf_1 = extruded[4].second;
    int outlet_surf_2 = extruded[5].second; // Rotated face (outlet)
    int wall1 = extruded[2].second;  // Side wall 1
    int wall2 = extruded[3].second;  // Side wall 2
    int wedge_face_1 = extruded[0].second;
    int wedge_face_2 = extruded[1].second;

    gmsh::model::addPhysicalGroup(2, {inlet_surf}, 1);
    gmsh::model::addPhysicalGroup(2, {outlet_surf_1, outlet_surf_2}, 2);
    gmsh::model::addPhysicalGroup(2, {wall1, wall2}, 3); // Walls
    gmsh::model::addPhysicalGroup(2, {wedge_face_1, wedge_face_2}, 4); // Wedge

    // Volume
    // Verify 3D elements exist
    std::vector<std::pair<int, int>> volumes;
    gmsh::model::getEntities(volumes, 3);
    if (volumes.empty()) {
        gmsh::logger::write("No 3D elements generated!", "error");
        return 1;
    }


    gmsh::model::addPhysicalGroup(3, {volumes[0].second}, 1);

    // Set names
    gmsh::model::setPhysicalName(2, 1, "inlet");
    gmsh::model::setPhysicalName(2, 2, "outlet");
    gmsh::model::setPhysicalName(2, 3, "walls");
    gmsh::model::setPhysicalName(2, 4, "wedge");
    // gmsh::model::setPhysicalName(3, 1, "fluid");

    // Синхронизируем и генерируем 3D-сетку
    gmsh::model::geo::synchronize();
    gmsh::option::setNumber("Mesh.MeshSizeMin", 0.1);
    gmsh::option::setNumber("Mesh.MeshSizeMax", 0.3);
    gmsh::model::mesh::generate(3);
    

    gmsh::option::setNumber("Mesh.MshFileVersion", 2.2);  // Use legacy format
    // Сохраняем сетку
    gmsh::write("laval.msh");

    // Просмотр в GUI (опционально)
    gmsh::fltk::run();
    gmsh::finalize();
    return 0;
}