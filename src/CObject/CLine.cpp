#include "CLine.h"

CREATE_RESULT CLine::Create(CPoint *pos){
    if(this->start == nullptr){
        this->start = pos;
        return CREATE_RESULT::ONESHOT;//あと1回
    }else{
        this->end   = pos;
        return CREATE_RESULT::COMPLETE;//完結
    }
}
bool CLine::isSelectable(Pos pos)const{
    //追加条件
    if(CObject::isSelectable(pos) &&
       std::min(this->start->x(),this->end->x()) - COLLISION_SIZE <= pos.x() &&
       std::max(this->start->x(),this->end->x()) + COLLISION_SIZE >= pos.x() &&
       std::min(this->start->y(),this->end->y()) - COLLISION_SIZE <= pos.y() &&
       std::max(this->start->y(),this->end->y()) + COLLISION_SIZE >= pos.y()){
        return true;
    }
    return false;
}


//中間点操作
CObject* CLine::GetChild(int index){
    if(index == 0)return this->start;
    if(index == 1)return this->end;
}
void     CLine::SetChild(int index,CObject* obj){
    if(index == 0)this->start = dynamic_cast<CPoint*>(obj);
    if(index == 1)this->end   = dynamic_cast<CPoint*>(obj);
}

int CLine::GetChildCount()const{
    return 2;
}
Pos  CLine::GetMiddleDivide(double t)const{
    return (*end - *start) * t + *start;
}

Pos CLine::GetNearPos(const Pos& hand)const{
    //点と直線の最近点
    return Pos::LineNearPoint(*this->start,*this->end,hand);
}
CEdge* CLine::Clone()const{
    CLine* ptr = new CLine(this->parent());
    ptr->start = new CPoint(*this->start,ptr);
    ptr->end   = new CPoint(*this->end  ,ptr);
    return ptr;
}

CLine::CLine(QObject *parent):
    CEdge(parent)
{
}
CLine::CLine(CPoint* start,CPoint* end,QObject* parent):
    CEdge(parent){
    this->SetChild(0,start);
    this->SetChild(1,end);
}



CLine::~CLine()
{
}

