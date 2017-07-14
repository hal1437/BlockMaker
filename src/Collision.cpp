
#include "Collision.h"

Pos Collision::GetHitPosFaceToLine(Pos face_norm,Pos face_center,Pos line_0,Pos line_s){
    double h = face_norm.DotPos(face_center);
    double cc =  ((h-face_norm.DotPos(line_0))/(face_norm.DotPos(line_s)));
    return (line_0+line_s * cc);
}

double Collision::GetLengthFaceToLine(CFace* face,Pos center ,Pos dir){
    //法線ベクトルの算出
    Pos norm = face->GetNorm();

    //面と平行
    if((dir).DotPos(norm) == 0)return false;

    //交点を取得
    Pos p = GetHitPosFaceToLine(norm,*face->GetPointSequence(0),center,dir);

    //四角形内であるか
    double sum=0;
    for(int i=0;i<face->edges.size();i++){
        sum += Pos::Angle(*face->GetPointSequence(i)-p,*face->GetPointSequence((i+1)%face->edges.size())-p);
    }
    if(std::abs(sum-360) > 0.000001){
        return -1;
    }else{
        return (center - p).Length();
    }
}

bool Collision::CheckHitFaceToLine(CFace* face,Pos center ,Pos dir){
    return (GetLengthFaceToLine(face,center,dir) != -1);
}

Collision::Collision(){}
Collision::~Collision(){}

