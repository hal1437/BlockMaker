#include "SmartDimension.h"


void SmartDimension::DrawArrow(QPainter& painter,const Pos& pos,const Pos& rote,double angle,double length)const{

    //基準ベクトル作成
    Pos base = rote.GetNormalize() * length;

    //行列に基づくベクトル回転
    Pos arrow1 = pos+Pos(base.x*cos( angle) - base.y*sin( angle),
                         base.x*sin( angle) + base.y*cos( angle));
    Pos arrow2 = pos+Pos(base.x*cos(-angle) - base.y*sin(-angle),
                         base.x*sin(-angle) + base.y*cos(-angle));

    painter.drawLine(pos.x,pos.y,arrow1.x,arrow1.y);
    painter.drawLine(pos.x,pos.y,arrow2.x,arrow2.y);
}


SmartDimension::DimensionType SmartDimension::GetDimensionType(CObject* obj1, CObject* obj2){
    if     (obj1->is<CLine >() && obj2 == nullptr   )return SmartDimension::length;
    else if(obj1->is<CPoint>() && obj2->is<CPoint>())return SmartDimension::distance;
    else if(obj1->is<CLine >() && obj2->is<CPoint>())return SmartDimension::distanceLine;
    else if(obj1->is<CPoint>() && obj2->is<CLine >())return SmartDimension::distanceLine;
    else if(obj1->is<CLine >() && obj2->is<CLine >())return SmartDimension::angle;
    else if(obj1->is<CArc  >() && obj2->is<CPoint>())return SmartDimension::radius;
    else if(obj1->is<CPoint>() && obj2->is<CArc  >())return SmartDimension::radius;
    else return SmartDimension::none;
}

void   SmartDimension::SetValue(double value){
    this->value = value;
}

double SmartDimension::GetValue()const{
    return this->value;
}


bool SmartDimension::SetTarget(CObject* obj1,CObject* obj2){
    this->type = GetDimensionType(obj1,obj2);
    target[0] = obj1;
    target[1] = obj2;

    if(!target[0]->is<CLine>() && target[1]->is<CLine>())std::swap(target[0],target[1]);
    return !(this->type == SmartDimension::none);
}

bool SmartDimension::Draw(QPainter& painter)const{
    const double PI = 3.141592;
    //描画
    if (this->type == none)return true;
    if (this->type == distanceLine){
        //矢印
        Pos d_pos[2];
        d_pos[0] = target[1]->GetJointPos(0);
        d_pos[1] = (target[0]->GetJointPos(1)-target[0]->GetJointPos(0)).GetNormalize();
        d_pos[1] = d_pos[1] * d_pos[1].GetNormalize().Dot(target[1]->GetJointPos(0)-target[0]->GetJointPos(0));
        d_pos[1] = d_pos[1] + target[0]->GetJointPos(0);

        painter.drawLine(d_pos[0].x,d_pos[0].y,d_pos[1].x,d_pos[1].y);
        //painter.drawLine(d_pos[1].x,d_pos[1].y,target[0]->GetJointPos(1).x,target[0]->GetJointPos(1).y);
        //painter.drawLine(d_pos[1].x,d_pos[1].y,target[1]->GetJointPos(1).x,target[1]->GetJointPos(1).y);
        this->DrawArrow(painter,d_pos[0],(d_pos[0]-d_pos[1]).GetNormalize(),3.0);
        this->DrawArrow(painter,d_pos[1],(d_pos[1]-d_pos[0]).GetNormalize(),3.0);
        //寸法の文字
        painter.save();
        QTransform trans;
        int bit = (d_pos[0].x - d_pos[1].x) > 0 ? 0 : 1;
        trans = QTransform::fromTranslate(((d_pos[0]+d_pos[1])/2).x,((d_pos[0]+d_pos[1])/2).y);
        trans.rotate(std::atan2(d_pos[0].y - d_pos[1].y,d_pos[0].x - d_pos[1].x)*180/PI + bit * 180);
        painter.setTransform(trans);
        painter.drawText(0,0,QString::number(value));
        painter.restore();
        return true;
    }

    return false;
}

Restraint* SmartDimension::MakeRestraint(){
    Restraint* answer;

    //点と直線
    if(type == SmartDimension::distance){
        Pos dot;
        Pos t0_0 = target[0]->GetJointPos(0);
        Pos t0_1 = target[0]->GetJointPos(1);
        Pos t1   = target[1]->GetJointPos(0);
        dot = (t0_1 - t0_0).GetNormalize();
        dot = dot * dot.GetNormalize().Dot(t1-t0_0);
        dot = dot + t0_0;

        answer = new LineRestraint(t1,t0_1-t0_0);
    }
    //if(type == SmartDimension::)
    return answer;
}


SmartDimension::SmartDimension(){
}

