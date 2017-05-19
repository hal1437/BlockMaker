#include "CLine.h"

CREATE_RESULT CLine::Create(CPoint *pos){
    if(this->start == nullptr){
        this->start = pos;
    }else{
        this->end   = pos;
    }
    return CREATE_RESULT::TWOSHOT;//完結
}
bool CLine::Draw(QPainter& painter)const{
    //描画
    painter.drawLine(*this->start,*this->end);
    return true;
}
bool CLine::Move(const Pos& diff){
    //ジョイントそれぞれを動かす
    if(!this->start->isLock())this->start->Move(diff);
    if(!this->end  ->isLock())this->end  ->Move(diff);
    return true;
}
void CLine::Lock(bool lock){
    //それぞれロック
    this->start->Lock(lock);
    this->end  ->Lock(lock);
    //自分もロック
    CObject::Lock(lock);
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


Pos CLine::GetNear(const Pos& hand)const{
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


CLine::~CLine()
{
}

