#include "CPoint.h"

bool CPoint::Create(Relative<Pos> pos,int){
    this->diff = pos.getDifference();
    this->ref = pos.getReference();
    return true;
}

Pos CPoint::GetNear(const Pos&)const{
    return this->getRelative();
}

bool CPoint::Draw(QPainter& painter)const{
    if     (this == CObject::selected )painter.setPen(QPen(Qt::cyan, 2));
    else if(this == CObject::selecting)painter.setPen(QPen(Qt::red , 2));
    else painter.setPen(QPen(Qt::blue, 2));
    Pos p = this->getRelative();
    painter.drawArc(p.x-DRAWING_CIRCLE_SIZE,
                    p.y-DRAWING_CIRCLE_SIZE,
                    2*DRAWING_CIRCLE_SIZE,
                    2*DRAWING_CIRCLE_SIZE,0,360*16);
    return true;
}
bool CPoint::Selecting(){
    return ((*this)().Length(mouse_over) < COLLISION_SIZE);
}

bool CPoint::isLocked(){
    return (this->ref != nullptr);
}

bool CPoint::Move(const Pos& diff){
    this->setDifferent(this->getDifference()+diff);
    return true;
}

int CPoint::GetJointNum()const{
    return 1;
}
Pos CPoint::GetJointPos(int index)const{
    if(index == 0) return (*this)();
    else return Pos();
}



CPoint::CPoint()
{

}

CPoint::~CPoint()
{

}

