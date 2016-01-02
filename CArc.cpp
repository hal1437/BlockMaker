#include "CArc.h"

bool CArc::Refresh(){
    QPoint c;
    if (is_Creating){
        center = (this->pos[0]() - CObject::mouse_over) / 2 + CObject::mouse_over;
        round  = (this->pos[0]()-center()).Length();
    }else{
        for(int i=0;i<2;i++){
            if(std::abs(round - (this->pos[i]()-center()).Length()) > 0.001){
                round = (this->pos[i]()-center()).Length();
                break;
            }
        }

        //点を円形修正
        for(int i=0;i<2;i++){
            if(this->pos[i].getReference()!=nullptr){
                pos[i].getReference()->diff = Pos::CircleNearPoint(center(),round,this->pos[i]());
            }
        }
    }

    return true;
}
bool CArc::Create(Relative<Pos> pos, int index){
    if(0 <= index && index < 2){
        this->pos[index] = pos;
        if(index==1){
            this->is_Creating = false;
            Refresh();
            return true;
        }else{
            this->is_Creating = true;
            return false;
        }
    }
    return false;
}

double CArc::GetRound()const{
    return round;
}
Pos    CArc::GetCenter()const{
    return center();
}
Pos CArc::GetNear(const Pos& hand)const{
    return Pos::CircleNearPoint(center(),round,hand);
}

bool CArc::Draw(QPainter& painter)const{
    if     (exist(CObject::selected,this))painter.setPen(QPen(Qt::cyan, DRAWING_LINE_SIZE));
    else if(this == CObject::selecting)painter.setPen(QPen(Qt::red , DRAWING_LINE_SIZE));
    else                               painter.setPen(QPen(Qt::blue, DRAWING_LINE_SIZE));
    Pos end_point;
    if(is_Creating){
        end_point = this->mouse_over;
    }else{
        end_point = pos[1]();
    }

    painter.drawArc(center().x-3,center().y-3,6,6,0,360*16);

    Pos dir1 = end_point-center();
    Pos dir2 = pos[0]()-center();
    double angle1 = std::atan2(-dir1.y,dir1.x) * 180 / PI;
    double angle2 = 360 - angle1 + (std::atan2(-dir2.y,dir2.x))*180 / PI;

    painter.drawArc (center().x-round,
                     center().y-round,
                     round*2,
                     round*2,
                     angle1*16,
                     Mod(angle2,(360))*16);


    return true;
}
bool CArc::Selecting(){
    //円と点の距離のアルゴリズム
    float d;

    Pos dir1 = pos[0]() - center();
    Pos dir2 = pos[1]() - center();
    Pos dir3 = CObject::mouse_over - center();
    double angle1  = std::atan2(-dir1.y,dir1.x) * 180 / PI;
    double angle2  = std::atan2(-dir2.y,dir2.x) * 180 / PI;
    double angle_m = std::atan2(-dir3.y,dir3.x) * 180 / PI;
    if(angle1 > angle2)std::swap(angle1,angle2);

    d = dir3.Length();
    if((std::abs(d-round) < COLLISION_SIZE) &&
       (angle1 <= angle_m && angle_m <= angle2)){
        return true;
    }else{
        return false;
    }
}

bool CArc::isLocked(){
    return false;
}
bool CArc::Move(const Pos& diff){
    for(int i = 0;i<2;i++){
        pos[i].getReference()->diff += diff;
    }
    center.diff += diff;
    //Refresh();
    return true;
}

int CArc::GetJointNum()const{
    return 2;
}
Pos CArc::GetJointPos(int index)const{
    if(index == -1)return center();
    else return pos[index]();
}


CArc::CArc()
{

}

CArc::~CArc()
{

}

