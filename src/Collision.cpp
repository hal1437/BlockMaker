
#include "Collision.h"

Pos Collision::GetHitPosFaceToLine(Pos face_norm,Pos face_center,Pos line_0,Pos line_s){
    double h = face_norm.DotPos(face_center);
    double cc =  ((h-face_norm.DotPos(line_0))/(face_norm.DotPos(line_s)));
    return (line_0+line_s * cc);
}

double Collision::GetLengthFaceToLine(CFace* face, Line line){
    //法線ベクトルの算出
    Pos norm = face->GetNorm();

    //面と平行
    if((line.pos2 - line.pos1).DotPos(norm) == 0)return false;

    //交点を取得
    Pos p = GetHitPosFaceToLine(norm,*face->GetPoint()[0],line.pos1,line.pos2-line.pos1);

    //四角形内であるか
    double sum=0;
    for(int i=0;i<face->GetPoint().size();i++){
        sum += Pos::Angle(*face->GetPoint()[i]-p,*face->GetPoint()[(i+1)%face->GetPoint().size()]-p);
    }
    if(std::abs(sum-360) > 0.000001)return -1;
    else return (line.pos1 - p).Length();
}

bool Collision::CheckHitFaceToLine(CFace* face, Line line){
    return (GetLengthFaceToLine(face,line) != -1);
}

Collision::Collision(){}
Collision::~Collision(){}

