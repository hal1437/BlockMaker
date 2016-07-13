#include "CRect.h"

#include "CLine.h"

#include <iostream>

bool CRect::Create(CPoint *hand, int index){
    if(0 <= index && index < 2){
        if(index==1){
            is_Creating = false;
            //構築
            for(int i=0;i<4;i++)lines[i] = new CLine();
            pos[1] = new CPoint();
            pos[2] = new CPoint();
            pos[3] = hand;
            if((GetJointPos(0).x < GetJointPos(3).x && GetJointPos(0).y < GetJointPos(3).y )||
               (GetJointPos(0).x > GetJointPos(3).x && GetJointPos(0).y > GetJointPos(3).y )){
                pos[1]->setDifferent(Pos(std::min(GetJointPos(0).x,GetJointPos(3).x),std::max(GetJointPos(0).y,GetJointPos(3).y)));
                pos[2]->setDifferent(Pos(std::max(GetJointPos(0).x,GetJointPos(3).x),std::min(GetJointPos(0).y,GetJointPos(3).y)));
            }else{
                pos[1]->setDifferent(Pos(std::max(GetJointPos(0).x,GetJointPos(3).x),std::max(GetJointPos(0).y,GetJointPos(3).y)));
                pos[2]->setDifferent(Pos(std::min(GetJointPos(0).x,GetJointPos(3).x),std::min(GetJointPos(0).y,GetJointPos(3).y)));
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
            this->is_Creating = true;
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
    this->is_Locking = lock;
}

bool CRect::Draw(QPainter& painter)const{
    if(is_Creating){
        painter.drawRect(std::min((*pos[0])().x,CPoint::mouse_over.x),
                         std::min((*pos[0])().y,CPoint::mouse_over.y),
                         std::max((*pos[0])().x,CPoint::mouse_over.x)-std::min((*pos[0])().x,CPoint::mouse_over.x),
                         std::max((*pos[0])().y,CPoint::mouse_over.y)-std::min((*pos[0])().y,CPoint::mouse_over.y));
    }
    return true;
}
bool CRect::Selecting(){
    return false;
}

bool CRect::Move(const Pos&){
    return true;
}

int CRect::GetJointNum()const{
    return 4;
}
Pos CRect::GetJointPos(int index)const{
    if(index == 3 && isCreateing())return CObject::mouse_over;
    return pos[index]->getRelative();
}
CPoint* CRect::GetJoint(int index){
    return pos[index];
}
std::vector<CObject*> CRect::GetChild(){
    std::vector<CObject*> answer;
    if(this->isCreateing())return answer;
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

