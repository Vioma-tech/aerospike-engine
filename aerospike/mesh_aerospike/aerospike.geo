// Parameters
ls = 1e-1;
angle_deg = 1.0;  // angle of the wedge
angle_rad = angle_deg * Pi / 180.0;
L_throat = 1.0;
L_exit = 3.0;
H_throat = 0.7;
H_exit = 0.1;
L_box = 2.0;

// Create 2D profile (nozzle contour)
Point(1) = {0.0, 0.5, 0.0, ls};          // Inlet (top)
Point(2) = {L_throat, H_throat, 0.0, ls}; // Throat
Point(3) = {L_exit, H_exit, 0.0, ls};     // Nozzle exit
Point(4) = {L_exit + L_box, H_exit, 0.0, ls}; // Outlet box (top)
Point(5) = {L_exit + L_box, 0.0, 0.0, ls};    // Outlet box (bottom)
Point(6) = {L_exit, 0.0, 0.0, ls};        // Nozzle exit (bottom)
Point(7) = {L_throat, 0.0, 0.0, ls};      // Throat (bottom)
Point(8) = {0.0, 0.0, 0.0, ls};           // Inlet (bottom)

// Connect points with lines
Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 5};
Line(5) = {5, 6};
Line(6) = {6, 7};
Line(7) = {7, 8};
Line(8) = {8, 1};

// Create curve loop and surface
Curve Loop(1) = {1, 2, 3, 4, 5, 6, 7, 8};
Plane Surface(1) = {1};

// Rotate surface around X axis by -angle_rad/2
Rotate {{1, 0, 0}, {0, 0, 0}, -angle_rad/2} { Surface{1}; }

// Revolve surface around X axis by angle_rad to create wedge
out[] = Extrude {{1, 0, 0}, {0, 0, 0}, angle_rad} { Surface{1}; };

// Physical groups
Physical Surface("inlet") = {out[6]};
Physical Surface("outlet") = {out[4], out[5]};
Physical Surface("walls") = {out[2], out[3]};
Physical Surface("wedge1") = {1};  // Original surface
Physical Surface("wedge2") = {out[0]};  // Other side of the wedge

// Volume
Physical Volume("fluid") = {out[1]};

// Generate mesh
Mesh 3;
// Mesh.MeshSizeMin = 0.01;
// Mesh.MeshSizeMax = 0.03;

// Save mesh in legacy format for OpenFOAM
// Mesh.MshFileVersion = 2.2;
// Save "aerospike.msh";