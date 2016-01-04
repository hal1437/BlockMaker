#include "CPoint.h"

bool CPoint::Create(Relative<Pos> pos,int){
    this->diff = pos.getDifference();
    this->ref = pos.getReference();
    this->is_Creating = false;
    return true;
}

Pos CPoint::GetNear(const Pos&)const{
    return this->getRelative();
}

bool CPoint::Draw(QPainter& painter)const{
    if     (exist(CObject::selected,this))painter.setPen(QPen(Qt::cyan, 2));
    else if(this == CObject::selecting)painter.setPen(QPen(Qt::red , 2));
    else painter.setPen(QPen(Qt::blue, 2));
    Pos p = this->getRelative();
    if(control_point){
        //四角
        QLine ps[4] = {QLine(p.x-DRAWING_CIRCLE_SIZE,p.y-DRAWING_CIRCLE_SIZE,p.x+DRAWING_CIRCLE_SIZE,p.y-DRAWING_CIRCLE_SIZE),
                       QLine(p.x+DRAWING_CIRCLE_SIZE,p.y-DRAWING_CIRCLE_SIZE,p.x+DRAWING_CIRCLE_SIZE,p.y+DRAWING_CIRCLE_SIZE),
                       QLine(p.x+DRAWING_CIRCLE_SIZE,p.y+DRAWING_CIRCLE_SIZE,p.x-DRAWING_CIRCLE_SIZE,p.y+DRAWING_CIRCLE_SIZE),
                       QLine(p.x-DRAWING_CIRCLE_SIZE,p.y+DRAWING_CIRCLE_SIZE,p.x-DRAWING_CIRCLE_SIZE,p.y-DRAWING_CIRCLE_SIZE)};
        painter.drawLines(ps,4);
    }else{
        //まる
        painter.drawArc(p.x-DRAWING_CIRCLE_SIZE,
                        p.y-DRAWING_CIRCLE_SIZE,
                        2*DRAWING_CIRCLE_SIZE,
                        2*DRAWING_CIRCLE_SIZE,0,360*16);
    }
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

bool CPoint::isSelectable()const{
    return this->selectable;
}
bool CPoint::isControlPoint()const{
    return this->control_point;
}

bool CPoint::Selectable(bool f){
    selectable = f;
}
bool CPoint::ControlPoint(bool f){
    control_point = f;
}



CPoint::CPoint()
{

}

CPoint::~CPoint()
{

}

