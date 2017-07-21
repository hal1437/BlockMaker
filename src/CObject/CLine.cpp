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
bool CLine::Draw(QPainter& painter)const{
    if(!this->isVisible())return true;
    //描画
    painter.drawLine(QPointF(this->start->x(),this->start->y()),
                     QPointF(this->end->x(),this->end->y()));
    return true;
}
bool CLine::Move(const Pos& diff){
    //ジョイントそれぞれを動かす
    if(!this->start->isLock())this->start->Move(diff);
    if(!this->end  ->isLock())this->end  ->Move(diff);
    return true;
}
void CLine::SetLock(bool lock){
    //それぞれロック
    this->start->SetLock(lock);
    this->end  ->SetLock(lock);
    //自分もロック
    CObject::SetLock(lock);
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
int CLine::GetMiddleCount()const{
    return 0;
}
CPoint* CLine::GetMiddle(int)const{
    return nullptr;
}
void CLine::SetMiddle(CPoint*,int){
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
    this->SetStartPos(start);
    this->SetEndPos(end);
}



CLine::~CLine()
{
}

