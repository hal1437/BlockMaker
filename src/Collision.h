#ifndef COLLISION_H
#define COLLISION_H

#include "Point.h"
#include "CObject/CFace.h"
#include <qdebug.h>

//線
struct Line{
    Pos pos1;
    Pos pos2;
};

class Collision
{
public:
    //平面の法線、平面上の任意の点、直線の始点、直線の向き
    static Pos GetHitPosFaceToLine(Pos face_norm,Pos face_center,Pos line_0,Pos line_s);

public:
    static double GetLengthFaceToLine(CFace* face, struct Line line);
    static bool   CheckHitFaceToLine (CFace* face, struct Line line);

    Collision();
    ~Collision();
};

#endif // COLLISION_H
