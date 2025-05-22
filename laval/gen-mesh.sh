rm -r constant/polyMesh 0
gmsh mesh/laval_structured.geo -3 -o laval.msh
gmshToFoam laval.msh
python3 update-boundary.py
cp -r 0.orig 0