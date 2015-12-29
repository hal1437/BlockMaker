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
    if     (obj1->is<CLine >() && obj2 == nullptr   )return SmartDimension::distance1;
    else if(obj1->is<CPoint>() && obj2->is<CPoint>())return SmartDimension::distance2;
    else if(obj1->is<CLine >() && obj2->is<CPoint>())return SmartDimension::distance2;
    else if(obj1->is<CPoint>() && obj2->is<CLine >())return SmartDimension::distance2;
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
    return !(this->type == SmartDimension::none);
}

bool SmartDimension::Draw(QPainter& painter)const{
    const double PI = 3.141592;
    //描画
    if (this->type == none)return true;
    if (this->type == distance1 || this->type == distance2){
        //矢印
        Pos center[2];
        for(int i=0;i<2;i++){
            if     (this->target[i]->is<CPoint>())center[i] = this->target[i]->GetJointPos(0);
            else if(this->target[i]->is<CLine >())center[i] = (this->target[i]->GetJointPos(0) + this->target[i]->GetJointPos(1))/2;
            else center[i] = this->target[i]->GetJointPos(0);

        }
        painter.drawLine(center[0].x,center[0].y,center[1].x,center[1].y);
        this->DrawArrow(painter,center[0],(center[0]-center[1]).GetNormalize(),3.0);
        this->DrawArrow(painter,center[1],(center[1]-center[0]).GetNormalize(),3.0);

        //寸法の文字
        painter.save();
        QTransform trans;
        int bit = (center[0].x - center[1].x) > 0 ? 0 : 1;
        trans = QTransform::fromTranslate(((center[0]+center[1])/2).x,((center[0]+center[1])/2).y);
        trans.rotate(std::atan2(center[0].y - center[1].y,center[0].x - center[1].x)*180/PI + bit * 180);
        painter.setTransform(trans);
        painter.drawText(0,0,QString::number(value));
        painter.restore();
        return true;
    }

    return false;
}

SmartDimension::SmartDimension(){
}

