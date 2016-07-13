#include "CLine.h"

#include <iostream>

bool CLine::Create(CPoint *pos, int index){
    if(0 <= index && index < 2){
        this->pos[index] = pos;
        if(index==1){
            this->is_Creating = false;
            return true;
        }else{
            this->is_Creating = true;
            return false;
        }
    }
    return false;
}

Pos CLine::GetNear(const Pos& hand)const{
    return Pos::LineNearPoint(GetJointPos(0),GetJointPos(1),hand);
}

bool CLine::Draw(QPainter& painter)const{
    if(is_Creating){
        //製作中
        painter.drawLine(QPointF(GetJointPos(0).x,GetJointPos(0).y),
                         QPointF(this->mouse_over.x,this->mouse_over.y));
    }else{
        //制作済み
        painter.drawLine(QPointF(GetJointPos(0).x,GetJointPos(0).y),
                         QPointF(GetJointPos(1).x,GetJointPos(1).y));
        //ロック
        if(this->isLocked()){
            Pos p = (GetJointPos(0) - GetJointPos(1))/2 + GetJointPos(1);
            painter.drawImage(p.x+10,p.y-10,QImage(":/Restraint/FixRestraint.png"));
        }
    }
    return true;
}
bool CLine::Selecting(){
    //直線と点の距離のアルゴリズムによって選択を決定
    float d;
    Pos v1,v2;
    v1 = GetJointPos(1) - GetJointPos(0);
    v2 = mouse_over     - GetJointPos(0);
    Pos near = Pos::LineNearPoint(Pos(),v1,v2);
    d = (near - v2).Length();

    if(d < COLLISION_SIZE && ((GetJointPos(0).x < mouse_over.x && mouse_over.x < GetJointPos(1).x) ||
                              (GetJointPos(1).x < mouse_over.x && mouse_over.x < GetJointPos(0).x) ||
                              (GetJointPos(0).y < mouse_over.y && mouse_over.y < GetJointPos(1).y) ||
                              (GetJointPos(1).y < mouse_over.y && mouse_over.y < GetJointPos(0).y))){
        return true;
    }else{
        return false;
    }
}

void CLine::Lock(bool lock){
    //それぞれロック
    pos[0]->Lock(lock);
    pos[1]->Lock(lock);
    this->is_Locking = lock;
}

bool CLine::Move(const Pos& diff){
    for(int i = 0;i<2;i++){
        if(!pos[i]->isLocked()){
            if(pos[i]->getReference() != nullptr)pos[i]->getReference()->diff += diff;
            else pos[i]->diff += diff;
        }
    }
    return true;
}


int CLine::GetJointNum()const{
    return 2;
}
Pos CLine::GetJointPos(int index)const{
    if(index == 1 && isCreateing())return CObject::mouse_over;
    return pos[index]->getRelative();
}
CPoint* CLine::GetJoint(int index){
    return pos[index];
}
std::vector<CObject*> CLine::GetChild(){
    std::vector<CObject*> ans;
    if(this->isCreateing())return ans;
    ans.push_back(pos[0]);
    ans.push_back(pos[1]);
    return ans;
}



CLine::CLine()
{

}

CLine::~CLine()
{

}

