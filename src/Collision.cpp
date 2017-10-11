
#include "Collision.h"

Pos Collision::GetHitPosFaceToLine(Pos face_norm,Pos face_center,Pos line_0,Pos line_s){
    double h = face_norm.DotPos(face_center);
    double cc =  ((h-face_norm.DotPos(line_0))/(face_norm.DotPos(line_s)));
    return (line_0+line_s * cc);
}

Pos Collision::GetHitPosFaceToPoint(Pos face_norm,Pos face_center,Pos pos){
    //A-N*(PA・N)
    double PA_N = (pos-face_center).DotPos(face_norm);
    return (pos - (face_norm * PA_N));
}


Collision::Collision(){}
Collision::~Collision(){}

