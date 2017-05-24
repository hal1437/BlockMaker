#include "Collision.h"


Pos Collision::GetHitPosFaceToLine(Pos face_norm,Pos face_center,Pos line_0,Pos line_s){
    double h = face_norm.DotPos(face_center);
    double cc =  ((h-face_norm.DotPos(line_0))/(face_norm.DotPos(line_s)));
    return (line_0+line_s * cc);
}

bool Collision::CheckHitFaceToLine(Face face,Line line){
    //法線ベクトルの算出
    Pos norm = Pos((face.corner[1]-face.corner[0]).Cross(face.corner[3]-face.corner[0])).GetNormalize();

    //面と平行
    if((line.pos2 - line.pos1).DotPos(norm) == 0)return false;

    //交点を取得
    Pos p = GetHitPosFaceToLine(norm,face.corner[0],line.pos1,line.pos2-line.pos1);

    //四角形内であるか
    double sum=0;
    for(int i=0;i<4;i++){
        sum += Pos::Angle(face.corner[i]-p,face.corner[(i+1)%4]-p);
    }
    qDebug() << sum;
    return std::abs(sum-360) < 0.000001;
}

Collision::Collision()
{

}

Collision::~Collision()
{

}

