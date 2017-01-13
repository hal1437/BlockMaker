#include "CRect.h"

#include "CLine.h"

#include <iostream>

bool CRect::Create(CPoint *hand, int index){
    if(0 <= index && index < 2){
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
            lines[0]->Make(GetJoint(0),0);
            lines[0]->Make(GetJoint(1),1);
            lines[1]->Make(GetJoint(0),0);
            lines[1]->Make(GetJoint(2),1);
            lines[2]->Make(GetJoint(2),0);
            lines[2]->Make(GetJoint(3),1);
            lines[3]->Make(GetJoint(1),0);
            lines[3]->Make(GetJoint(3),1);
            return true;
        }else{
            this->pos[0] = hand;
            return false;
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
    if(this->isCreating()){
        QPoint p1(pos[0]->x            ,pos[0]->y);
        QPoint p2(pos[0]->x            ,CPoint::mouse_over.y);
        QPoint p3(CPoint::mouse_over.x ,pos[0]->y);
        QPoint p4(CPoint::mouse_over.x ,CPoint::mouse_over.y);

        painter.drawLine(p1,p2);
        painter.drawLine(p1,p3);
        painter.drawLine(p2,p4);
        painter.drawLine(p3,p4);
    }
    return true;
}
bool CRect::isSelectable()const{
    return false;
}

bool CRect::Move(const Pos&){
    return true;
}

int CRect::GetJointNum()const{
    return 4;
}
Pos CRect::GetJointPos(int index)const{
    if(index == 3 && this->isCreating())return CObject::mouse_over;
    return *pos[index];
}
CPoint* CRect::GetJoint(int index){
    return pos[index];
}
std::vector<CObject*> CRect::GetChild(){
    std::vector<CObject*> answer;
    if(this->isCreating())return answer;
    for(int i=0;i<4;i++){
        answer.push_back(lines[i]);
        std::vector<CObject*> children = lines[i]->GetChild();
        for(CObject* child :children)answer.push_back(child);
    }
    return answer;
}

CRect::CRect()
{

}

CRect::~CRect()
{

}

