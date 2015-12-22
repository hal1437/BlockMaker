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
    if(CObject::selecting)painter.setPen(QPen(Qt::red, 2));
    else painter.setPen(QPen(Qt::blue, 2));
    Pos p = this->getRelative();
    painter.drawArc(p.x-DRAWING_CIRCLE_SIZE,
                    p.y-DRAWING_CIRCLE_SIZE,
                    2*DRAWING_CIRCLE_SIZE,
                    2*DRAWING_CIRCLE_SIZE,0,360*16);
    return true;
}
bool CPoint::Selecting(){
    if((*this)().Length(mouse_over) < COLLISION_SIZE){
        selecting=true;
        return true;
    }else{
        selecting=false;
        return false;
    }
}

bool CPoint::isLocked(){
    return (this->ref != nullptr);
}

bool CPoint::Move(const Pos& diff){
    this->setDifferent(this->getDifference()+diff);
}



CPoint::CPoint()
{

}

CPoint::~CPoint()
{

}

