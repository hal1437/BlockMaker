#ifndef COLLISION_H
#define COLLISION_H

#include "Point.h"
#include "CObject/CFace.h"
#include <qdebug.h>

//線

class Collision
{
public:
    //線分と平面の交点を取得（平面の法線、平面上の任意の点、直線の始点、直線の向き）
    static Pos GetHitPosFaceToLine(Pos face_norm,Pos face_center,Pos line_0,Pos line_s);

    //点と平面の最近点を取得（平面の法線、平面上の任意の点、点の座標）
    static Pos GetHitPosFaceToPoint(Pos face_norm,Pos face_center,Pos pos);

    //点が平面に存在するかチェック（平面の法線、平面上の任意の点、点の座標）
    static bool ChackPointOnFace(Pos face_norm,Pos face_center,Pos pos);

public:

    Collision();
    ~Collision();
};

#endif // COLLISION_H
