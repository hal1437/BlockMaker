#include "CPoint.h"

bool CPoint::Create(CPoint* pos,int){
    (*this) = *pos;
    return true;
}

Pos CPoint::GetNear(const Pos&)const{
    return this->getRelative();
}

bool CPoint::Draw(QPainter& painter)const{
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
    if(this->isLocked()){
        painter.drawImage(p.x+10,p.y-10,QImage(":/Restraint/FixRestraint.png"));
    }
    return true;
}
bool CPoint::Selecting(){
    return ((*this)().Length(CPoint::mouse_over) < COLLISION_SIZE);
}


bool CPoint::Move(const Pos& pos){
    if(!isLocked())diff = this->getRelative() + pos;
    return true;
}

int CPoint::GetJointNum()const{
    return 1;
}
Pos CPoint::GetJointPos(int index)const{
    if(index == 0) return (*this)();
    else return Pos();
}
CPoint* CPoint::GetJoint(int){
    return this;
}

bool CPoint::isCreateing()const{
    return false;
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



CPoint::CPoint(){
}

CPoint::CPoint(const Pos &pos):
    Relative<Pos>(pos){

}

CPoint::CPoint(double x,double y):
    Relative<Pos>(Pos(x,y)){

}

CPoint::~CPoint()
{

}

