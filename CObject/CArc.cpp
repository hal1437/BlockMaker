
#include "CArc.h"

bool CArc::Refresh(){
    if (this->isCreating()){
        *center = (GetJointPos(0) - CObject::mouse_over) / 2 + CObject::mouse_over;
        round = (GetJointPos(0) - GetCenter()).Length();
    }else{
        for(int i=0;i<2;i++){
            if(std::abs(round - (GetJointPos(i)-GetCenter()).Length()) > 0.001){
                round = (GetJointPos(i)-GetCenter()).Length();
                break;
            }
        }

        //点を円形修正
        for(int i=0;i<2;i++){
            pos[i]->Move(Pos::CircleNearPoint(GetCenter(),round,GetJointPos(i))-GetJointPos(i));
        }
    }

    return true;
}
bool CArc::Create(CPoint* pos, int index){
    if(0 <= index && index < 2){
        this->pos[index] = pos;
        if(index==1){
            Refresh();
            *center = (GetJointPos(0) - GetJointPos(1)) / 2 + GetJointPos(1);
            round = (GetJointPos(0) - GetCenter()).Length();

            return true;
        }else{
            return false;
        }
    }
    return false;
}

double CArc::GetRound()const{
    return round;
}
Pos    CArc::GetCenter()const{
    return *this->center;
}
Pos CArc::GetNear(const Pos& hand)const{
    return Pos::CircleNearPoint(GetCenter(),round,hand);
}
void CArc::Lock(bool lock){
    //それぞれロック
    pos[0]->Lock(lock);
    pos[1]->Lock(lock);
    center->Lock(lock);
    this->Lock(lock);
}

bool CArc::Draw(QPainter& painter, QTransform trans)const{
    Pos end_point;
    if(this->isCreating()){
        end_point = this->mouse_over;
    }else{
        end_point = GetJointPos(1);
    }

    Pos dir1 = (end_point      - GetCenter());
    Pos dir2 = (GetJointPos(0) - GetCenter());
    dir1.Transform(trans);
    dir2.Transform(trans);
    double angle1 = std::atan2(-dir1.y,dir1.x) * 180 / PI;
    double angle2 = 360 - angle1 + (std::atan2(-dir2.y,dir2.x))*180 / PI;

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
    Pos dir3 = CObject::mouse_over - GetCenter();

    float d = dir3.Length();
    if((d                 < COLLISION_SIZE / CObject::Drawing_scale ||
       (std::abs(d-round) < COLLISION_SIZE / CObject::Drawing_scale)) &&
       (Pos::Angle(dir2,dir3) < Pos::Angle(dir2,dir1))){
        return true;
    }else{
        return false;
    }
    return false;
}

bool CArc::Move(const Pos& diff){
    for(int i = 0;i<2;i++){
        pos[i]->Move(diff);
    }
    center->Move(diff);
    return true;
}

int CArc::GetJointNum()const{
    return 2;
}
Pos CArc::GetJointPos(int index)const{
    if(index == -1)return GetCenter();
    else if(index == 1 && isCreating())return CObject::mouse_over;
    else return *pos[index];
}
CPoint* CArc::GetJoint(int index){
    if(index == 2)return center;
    else return pos[index];
}
std::vector<CObject*> CArc::GetChild(){
    std::vector<CObject*>ans;
    if(this->isCreating())return ans;
    ans.push_back(pos[0]);
    ans.push_back(pos[1]);
    ans.push_back(center);
    return ans;
}

CArc::CArc()
{
    this->center = new CPoint();
}

CArc::~CArc()
{
    delete center;
}

