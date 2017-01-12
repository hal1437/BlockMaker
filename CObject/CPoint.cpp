#include "CPoint.h"

bool CPoint::Create(CPoint* pos,int){
    (*this) = *pos;
    return true;//生成終了
}

Pos CPoint::GetNear(const Pos&)const{
    return *this;
}

bool CPoint::Draw(QPainter& painter)const{
    Pos center = *this;
    if(control_point){
        //四角
        QLine ps[4] = {QLine(center.x-DRAWING_CIRCLE_SIZE,center.y-DRAWING_CIRCLE_SIZE,center.x+DRAWING_CIRCLE_SIZE,center.y-DRAWING_CIRCLE_SIZE),
                       QLine(center.x+DRAWING_CIRCLE_SIZE,center.y-DRAWING_CIRCLE_SIZE,center.x+DRAWING_CIRCLE_SIZE,center.y+DRAWING_CIRCLE_SIZE),
                       QLine(center.x+DRAWING_CIRCLE_SIZE,center.y+DRAWING_CIRCLE_SIZE,center.x-DRAWING_CIRCLE_SIZE,center.y+DRAWING_CIRCLE_SIZE),
                       QLine(center.x-DRAWING_CIRCLE_SIZE,center.y+DRAWING_CIRCLE_SIZE,center.x-DRAWING_CIRCLE_SIZE,center.y-DRAWING_CIRCLE_SIZE)};
        painter.drawLines(ps,4);
    }else{
        //丸
        painter.drawArc(center.x-DRAWING_CIRCLE_SIZE,
                        center.y-DRAWING_CIRCLE_SIZE,
                        2*DRAWING_CIRCLE_SIZE,
                        2*DRAWING_CIRCLE_SIZE,0,360*16);
    }
    if(this->isLock()){
        painter.drawImage(center.x+10,center.y-10,QImage(":/Restraint/FixRestraint.png"));
    }
    return true;
}
bool CPoint::isSelectable()const{
    return (this->Length(CPoint::mouse_over) < COLLISION_SIZE);
}


bool CPoint::Move(const Pos& pos){
    if(!isLock())(*this) += pos;
    return true;
}

int CPoint::GetJointNum()const{
    return 1;
}
Pos CPoint::GetJointPos(int index)const{
    if(index == 0)return *this;
    else return Pos();
}
CPoint* CPoint::GetJoint(int){
    return this;
}

bool CPoint::isControlPoint()const{
    return this->control_point;
}

bool CPoint::ControlPoint(bool f){
    control_point = f;
}



CPoint::CPoint(){
}

CPoint::CPoint(const Pos &pos):
    Pos(pos){

}

CPoint::CPoint(double x,double y):
    Pos(Pos(x,y)){

}

CPoint::~CPoint()
{

}

