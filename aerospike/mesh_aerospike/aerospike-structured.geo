
// SetFactory("OpenCASCADE");
gridsize_s = 0.001;
gridsize_By = 0.0025;
gridsize_Bx = 0.002;
gridsize_B = 0.008;

x = {0.0, 0.05, 0.050922488300214876, 0.05184497660042974, 0.052767464900644614, 0.05368995320085948, 0.05461244150107435, 0.05553492980128922, 0.05645741810150409, 0.05737990640171896, 0.05830239470193383, 0.059224883002148704, 0.06014737130236357, 0.061069859602578436, 0.06199234790279331, 0.06291483620300818, 0.06383732450322305, 0.06475981280343791, 0.0656823011036528, 0.06660478940386766, 0.06752727770408253, 0.06844976600429739, 0.06937225430451227, 0.07029474260472714, 0.071217230904942, 0.07213971920515688, 0.07306220750537175, 0.07398469580558661, 0.0749071841058015, 0.07582967240601636, 0.07675216070623123, 0.07767464900644609, 0.07859713730666096, 0.07951962560687584, 0.0804421139070907, 0.08136460220730557, 0.08228709050752045, 0.08320957880773532, 0.08413206710795018, 0.08505455540816506, 0.08597704370837991, 0.0868995320085948, 0.08782202030880966, 0.08874450860902452, 0.0896669969092394, 0.09058948520945427, 0.09150622691845116, 0.09242296862744803, 0.15333971033644492};
y = {0.014634186784812389, 0.014634186784812389, 0.014645758241241563, 0.014680494483225177, 0.014738461336095958, 0.014819769204155953, 0.014924574130079943, 0.015053079311321063, 0.015205537108382038, 0.015382251592212008, 0.015583581692429606, 0.01580994502527984, 0.016061822501126984, 0.0163397638370706, 0.016644394132540784, 0.016976421706625285, 0.017336647448328697, 0.017725975998999006, 0.01814542917542568, 0.018596162160625256, 0.0190794831485976, 0.01959679822702122, 0.020129397095474552, 0.02066199596392789, 0.021194594832381224, 0.021727193700834556, 0.022259792569287895, 0.022792391437741234, 0.023324990306194567, 0.0238575891746479, 0.024390188043101238, 0.02492278691155457, 0.02545538578000791, 0.02598798464846124, 0.026520583516914574, 0.027040131615583562, 0.027501042467682323, 0.0279035302217017, 0.028251404168645695, 0.028547693514669716, 0.028794803546700113, 0.028994626909652686, 0.02914862348744643, 0.029257877513652265, 0.02932313749866273, 0.029344842575577068, 0.029258385845927198, 0.028989164883760216, -0.0};

L = x[#x[]-1];
R = 0.024517759651274465 + y[0];

Point(1) = {0, R, 0, gridsize_s};
Point(2) = {L, R, 0, gridsize_s};
// Point(3) = {L, 0, 0, gridsize_s};
// Point(4) = {0, R, 0, gridsize_s};

pList[0] = 1;
For i In {0: #x[]-1}
    pList[i] = newp;
    Point(pList[i]) = {x[i], y[i], 0, gridsize_s};
EndFor
BSpline(2001) = pList[];


o1 = 0.005;
o2 = 0.02;

A  = 6*R;
B  = 4*A;




Point(1001) = {L+B, 0, 0, gridsize_B};
Point(10010) = {L+B, R, 0, gridsize_B};
Point(10015) = {L+B, R+o1, 0, gridsize_B};

Point(1002) = {L+B, A, 0, gridsize_B};
Point(10025) = {L, A, 0, gridsize_B};
Point(1003) = {L-o2, A, 0, gridsize_B};
Point(1004) = {L-o2, R+o1, 0, gridsize_B};
Point(1005) = {L, R+o1, 0, gridsize_Bx};


Line(1) = {1, 2};
Line(2) = {2, pList[#pList[] - 1]};
Line(4) = {pList[0], 1};

Line(5) = {2, 10010};
Line(6) = {10010,1001};
Line(7) = {1001, pList[#pList[] - 1]};

Line(8) = {10010, 10015};
Line(9) = {10015, 1002};
Line(10) = {1002, 10025};
Line(11) = {10025, 1003};
Line(12) = {1003, 1004};
Line(13) = {1004, 1005};
Line(14) = {1005, 2};
Line(15) = {10015, 1005};
Line(16) = {10025, 1005};
// Line(14) = {10015, 1005};



Line Loop(101) = {1, 2, -2001, 4};
Line Loop(102) = {5, 6, 7, -2};
Line Loop(103) = {8, 15, 14, 5};
Line Loop(104) = {9, 10, 16, -15};
Line Loop(105) = {11, 12, 13, -16};
// // Line Loop(110) = {1, 5, 6, 8, 9, 10, 11, 12, -2001, 4};
// // Line Loop(104) = {}

Plane Surface(1) = 101;
Plane Surface(2) = 102;
Plane Surface(3) = 103;
Plane Surface(4) = 104;
Plane Surface(5) = 105;

Transfinite Line{4, 2} = Ceil(R/gridsize_s);
Transfinite Line{2001, 1} = Ceil(L/gridsize_s);


Transfinite Line{2, 6} = Ceil(R/gridsize_s);
Transfinite Line{5, 7} = Ceil((B)/gridsize_Bx);


Transfinite Line{7, 15} = Ceil((B)/gridsize_Bx);
Transfinite Line{8, 14} = Ceil((o1)/gridsize_s);

// Transfinite Line{15, 10} = Ceil((B)/gridsize_Bx);
// Transfinite Line{12, 16, 9} = Ceil((A - o1 - R)/gridsize_By);
// Transfinite Line{11, 13} = Ceil((o2)/gridsize_Bx);


// Transfinite Surface{1};
// Recombine Surface{1};


Transfinite Surface{1, 2, 3};
Recombine Surface{1, 2, 3, 4, 5};

// 
// Transfinite Surface{1, 2, 3, 4, 5};
// Recombine Surface{1, 2, 3, 4, 5};

Rotate {{1,0,0},{0,0,0},1*Pi/180.0}
{
	Surface{1,2,3, 4, 5};
    // Recombine;
}
extr[] = Extrude { {1,0.0,0}, {0,0.0,0.0},  -2*Pi/180.0} { 
    Surface{1, 2, 3, 4, 5}; 
    Layers{1};
    Recombine;
};
//Coherence;



// Color Brown {Surface{extr[24]};} // wall

Physical Surface("wedge0", 1) = {extr[0], extr[6], extr[11], extr[17], extr[23]};
Physical Surface("wedge1", 2) = {extr[1], extr[7], extr[12], extr[18], extr[24]};
Physical Surface("atm1", 3) = {extr[9], extr[13], extr[19]};
Physical Surface("atm2", 4) = {extr[20], extr[25]};
Physical Surface("atm3", 5) = {extr[26]};
Physical Surface("wall", 6) = {extr[2], extr[4], extr[15], extr[27]};
Physical Surface("inlet", 7) = {extr[5]};

//Physical Surface("test", 8) = {extr[28]};



Physical Volume("vol1", 10000) = {1, 2, 3, 4, 5};
// Physical Surface("inlet", 1001) = {inlet_s};
// // Physical Surface("outlet", 1002) = {outlet_s};
// // Physical Surface("wall", 1003) = {extr[3]};
// Physical Surface("wedge0", 1) = {extr[0], extr[5], extr[10]};
// Physical Surface("wedge1", 2) = {extr[1], extr[6], extr[11]};
// Physical Surface("atm1", 3) = {extr[7], extr[12], extr[13]};
// Physical Surface("atm2", 4) = {extr[14]};
// Physical Surface("atm3", 5) = {extr[15]};
// Physical Surface("wall", 7) = {extr[3], extr[16], extr[17]};
// Physical Surface("inlet", 8) = {extr[4]};