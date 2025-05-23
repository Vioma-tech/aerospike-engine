rm -r constant/polyMesh 0
gmsh mesh_aerospike/aerospike-structured.geo -3 -o aerospike.msh
gmshToFoam aerospike.msh
python3 update-boundary.py
cp -r 0.orig 0