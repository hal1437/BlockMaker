#include "CRect.h"

#include "CLine.h"

#include <iostream>

bool CRect::Create(CPoint *hand, int index){
    if(0 <= index && index < 2){
        if(index==0){
            this->pos[0] = hand;
            return false;
        }
        if(index==1){
            //構築
            for(int i=0;i<4;i++)lines[i] = new CLine();
            pos[1] = new CPoint();
            pos[2] = new CPoint();
            pos[3] = hand;
            if((GetJointPos(0).x < GetJointPos(3).x && GetJointPos(0).y < GetJointPos(3).y )||
               (GetJointPos(0).x > GetJointPos(3).x && GetJointPos(0).y > GetJointPos(3).y )){
                *pos[1] = Pos(std::min(GetJointPos(0).x,GetJointPos(3).x),std::max(GetJointPos(0).y,GetJointPos(3).y));
                *pos[2] = Pos(std::max(GetJointPos(0).x,GetJointPos(3).x),std::min(GetJointPos(0).y,GetJointPos(3).y));
            }else{
                *pos[1] = Pos(std::max(GetJointPos(0).x,GetJointPos(3).x),std::max(GetJointPos(0).y,GetJointPos(3).y));
                *pos[2] = Pos(std::min(GetJointPos(0).x,GetJointPos(3).x),std::min(GetJointPos(0).y,GetJointPos(3).y));
            }
            lines[0]->Create(GetJoint(0),0);
            lines[0]->Create(GetJoint(1),1);
            lines[1]->Create(GetJoint(0),0);
            lines[1]->Create(GetJoint(2),1);
            lines[2]->Create(GetJoint(2),0);
            lines[2]->Create(GetJoint(3),1);
            lines[3]->Create(GetJoint(1),0);
            lines[3]->Create(GetJoint(3),1);
            return true;
        }
    }
    return false;
}

Pos CRect::GetNear(const Pos& hand)const{
    return Pos::LineNearPoint(GetJointPos(0),GetJointPos(1),hand);
}
void CRect::Lock(bool lock){
    for(int i =0;i<4;i++){
        this->lines[i]->Lock(lock);
    }
    this->Lock(lock);
}

bool CRect::Draw(QPainter& painter)const{
    QPointF p1 = QPointF(GetJointPos(0).x,GetJointPos(0).y);
    QPointF p2 = QPointF(GetJointPos(1).x,GetJointPos(1).y);
    QPointF p3 = QPointF(GetJointPos(2).x,GetJointPos(2).y);
    QPointF p4 = QPointF(GetJointPos(3).x,GetJointPos(3).y);

    painter.drawLine(p1,p2);
    painter.drawLine(p1,p3);
    painter.drawLine(p2,p4);
    painter.drawLine(p3,p4);
    return true;
}
bool CRect::isSelectable()const{
    if(!this->isCreating()){
        for(int i=0;i<4;i++){
            if(this->lines[i]!=nullptr && this->lines[i]->isSelectable()){
                return true;
            }
        }
    }
    return false;
}

bool CRect::Move(const Pos& diff){
    for(int i=0;i<4;i++){
        this->pos[i]->Move(diff);
    }
}

CLine** CRect::GetLines(){
    return lines;
}

int CRect::GetJointNum()const{
    return 4;
}
Pos CRect::GetJointPos(int index)const{
    if(this->isCreating()){
        Pos pp[4] = {Pos(std::min(pos[0]->x,CObject::mouse_pos.x),std::min(pos[0]->y,CObject::mouse_pos.y)),
                     Pos(std::max(pos[0]->x,CObject::mouse_pos.x),std::min(pos[0]->y,CObject::mouse_pos.y)),
                     Pos(std::min(pos[0]->x,CObject::mouse_pos.x), std::max(pos[0]->y,CObject::mouse_pos.y)),
                     Pos(std::max(pos[0]->x,CObject::mouse_pos.x),std::max(pos[0]->y,CObject::mouse_pos.y))};
        return pp[index];
    }else{
        return *pos[index];
    }
}
CPoint* CRect::GetJoint(int index){
    return pos[index];
}

CRect::CRect()
{
    for(int i=0;i<4;i++)this->lines[i] = nullptr;
}

CRect::~CRect()
{

}

