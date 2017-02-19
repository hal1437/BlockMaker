
#include "CArc.h"

bool CArc::Refresh(){
    if (this->isCreating()){
        //中心を二点の中点に修正
        *pos[0] = (GetJointPos(1) - CObject::mouse_pos) / 2 + CObject::mouse_pos;
        //半径をいい感じに
        round = (GetJointPos(1) - GetCenter()).Length();
    }else{
        //ずれた点に半径を合わせる
        for(int i=1;i<3;i++){
            if(std::abs(round - (GetJointPos(i)-GetCenter()).Length()) > SAME_POINT_EPS){
                round = (GetJointPos(i)-GetCenter()).Length();
                //点を新しい半径に修正
                for(int j=1;j<3;j++){
                    pos[j]->Move(Pos::CircleNearPoint(GetCenter(),round,GetJointPos(j)) - GetJointPos(j));
                }
            }
        }
    }

    return true;
}
bool CArc::Create(CPoint* pos, int index){
    if(index == 0 || index == 1){
        this->pos[index+1] = pos;
        if(index==1){
            Refresh();   //円形修正
            return true; //終了
        }
    }
    return false;
}

double CArc::GetRound()const{
    return this->round;
}
Pos    CArc::GetCenter()const{
    return *this->pos[0];
}

void CArc::Lock(bool lock){
    //それぞれロック
    for(int i=0;i<3;i++){
        pos[i]->Lock(lock);
    }
}

bool CArc::Draw(QPainter& painter)const{
    Pos dir1 = (GetJointPos(2) - GetCenter());
    Pos dir2 = (GetJointPos(1) - GetCenter());
    double angle1 = std::atan2(-dir1.y,dir1.x) * 180 / PI;
    double angle2 = 360 - angle1 + (std::atan2(-dir2.y,dir2.x)) * 180 / PI;

    painter.drawArc (GetCenter().x-round,
                     GetCenter().y-round,
                     round*2,
                     round*2,
                     angle1*16,
                     Mod(angle2,(360))*16);


    return true;
}
bool CArc::isSelectable()const{
    //円と点の距離のアルゴリズム
    Pos dir1 = GetJointPos(0) - GetCenter();
    Pos dir2 = GetJointPos(1) - GetCenter();
    Pos dir3 = CObject::mouse_pos - GetCenter();

    float d = dir3.Length();
    if((d                 < COLLISION_SIZE / drawing_scale ||
       (std::abs(d-round) < COLLISION_SIZE / drawing_scale)) &&
       (Pos::Angle(dir2,dir3) < Pos::Angle(dir2,dir1))){
        return true;
    }else{
        return false;
    }
    return false;
}

bool CArc::Move(const Pos& diff){
    for(int i = 0;i<3;i++){
        pos[i]->Move(diff);
    }
    return true;
}

Pos CArc::GetNear(const Pos& hand)const{
    return Pos::CircleNearPoint(GetCenter(),round,hand);
}


int CArc::GetJointNum()const{
    return 3;
}
Pos CArc::GetJointPos(int index)const{
    if(index == 0){
        return GetCenter();
    }
    else if(index == 2 && isCreating()){ //作成時ならば
        return CObject::mouse_pos;
    }
    else {
        return *pos[index];
    }
}
CPoint* CArc::GetJoint(int index){
    return pos[index];
}

CArc::CArc()
{
    pos[0] = new CPoint();
}

CArc::~CArc()
{
    delete pos[0];
}

