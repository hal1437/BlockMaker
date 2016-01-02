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
    else if(obj1->is<CArc  >() && obj2 == nullptr   )return SmartDimension::radius;
    else if(obj1->is<CPoint>() && obj2->is<CPoint>())return SmartDimension::distance;
    else if(obj1->is<CLine >() && obj2->is<CPoint>())return SmartDimension::distanceLine;
    else if(obj1->is<CPoint>() && obj2->is<CLine >())return SmartDimension::distanceLine;
    else if(obj1->is<CLine >() && obj2->is<CLine >())return SmartDimension::angle;
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

    if(!target[0]->is<CLine>() && target[1]!=nullptr || target[1]->is<CLine>())std::swap(target[0],target[1]);
    return !(this->type == SmartDimension::none);
}

CObject* SmartDimension::GetTarget(int index)const{
    return this->target[index];
}


bool SmartDimension::Draw(QPainter& painter)const{
    //描画
    if (this->type == none)return true;
    if (this->type == distance){
        //矢印
        Pos t0 = target[0]->GetJointPos(0);
        Pos t1 = target[1]->GetJointPos(0);

        painter.drawLine(t0.x,t0.y,t1.x,t1.y);
        this->DrawArrow(painter,t0,(t0-t1).GetNormalize(),3.0);
        this->DrawArrow(painter,t1,(t1-t0).GetNormalize(),3.0);

        //寸法の文字
        painter.save();
        QTransform trans;
        int bit = (t0.x - t1.x) > 0 ? 0 : 1;
        trans = QTransform::fromTranslate(((t0+t1)/2).x,((t0+t1)/2).y);
        trans.rotate(std::atan2(t0.y - t1.y,t0.x - t1.x)*180/PI + bit * 180);
        painter.setTransform(trans);
        painter.drawText(0,0,QString::number(value));
        painter.restore();
        return true;
    }
    if (this->type == distanceLine){
        //矢印
        Pos lines[2] = {target[0]->GetJointPos(0),target[0]->GetJointPos(1)};
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

        //線からはみ出たら補助線を引く
        if(d_pos[1].y < std::min(lines[0].y,lines[1].y) ||  std::max(lines[0].y,lines[1].y) < d_pos[1].y){
            Pos near;//近い方
            near = std::max(lines[0],lines[1],[&](Pos l,Pos r){return ((l-d_pos[1]).Length() < (r-d_pos[1]).Length());});
            painter.drawLine(near.x,near.y,d_pos[1].x,d_pos[1].y);
        }

        /*
        //拘束補助線
        Pos dot;
        Pos t0_0 = target[0]->GetJointPos(0);
        Pos t0_1 = target[0]->GetJointPos(1);
        Pos t1   = target[1]->GetJointPos(0);
        dot = (t0_1 - t0_0).GetNormalize();
        dot = dot * dot.GetNormalize().Dot(t1-t0_0);
        dot = dot + t0_0;
        Pos c = (dot - (dot-t1).GetNormalize() * value);
        Pos r = t0_1-t0_0;
        painter.drawArc(c.x-10,c.y-10,20,20,0,360*16);
        painter.drawLine((c+r).x,(c+r).y,c.x,c.y);
        painter.drawLine((c-r).x,(c-r).y,c.x,c.y);
        */

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
    if (this->type == radius){
        //矢印
        CArc* arc = dynamic_cast<CArc*>(target[0]);
        Pos pos[2] = {arc->GetJointPos(0),arc->GetJointPos(1)};
        Pos center = arc->GetCenter();

        double angle = std::acos((pos[0]-center).GetNormalize().Dot(pos[1]-center) / arc->GetRound())*180/PI/2;
        if( Pos::MoreThan(pos[0],center,pos[1]) && pos[0].x < center.x)angle = 180 - angle;
        if(!Pos::MoreThan(pos[0],center,pos[1]) && pos[0].x > center.x)angle = 180 - angle;
        std::cerr << angle;
        Pos dir(QPoint(pos[0].x-center.x,pos[0].y-center.y)*QTransform().rotate(angle));
        Pos dir_p = dir + dir.GetNormalize()*50;

        painter.drawLine(center.x,center.y,(center+dir_p).x,(center+dir_p).y);
        DrawArrow(painter,(center+dir),-dir,3.0);

        //寸法の文字
        painter.save();
        QTransform trans;
        QString str = QString("R")+QString::number(value);
        QFontMetrics fm = painter.fontMetrics();
        int bit = (dir.x) > 0 ? 0 : 1;
        trans = QTransform::fromTranslate((center+dir).x,(center+dir).y);
        trans.rotate(std::atan2(dir.y,dir.x)*180/PI + bit * 180);
        painter.setTransform(trans);
        if(bit)painter.drawText(-(fm.width(str)+10),-2,str);
        else   painter.drawText(10,-2,str);
        painter.restore();
        return true;
    }

    return false;
}

Restraint* SmartDimension::MakeRestraint(){
    Restraint* answer;

    //点と直線
    if(type == SmartDimension::distanceLine){
        Pos dot;
        Pos t0_0 = target[0]->GetJointPos(0);
        Pos t0_1 = target[0]->GetJointPos(1);
        Pos t1   = target[1]->GetJointPos(0);
        dot = (t0_1 - t0_0).GetNormalize();
        dot = dot * dot.GetNormalize().Dot(t1-t0_0);
        dot = dot + t0_0;

        answer = new LineRestraint((dot - (dot-t1).GetNormalize() * value),t0_1-t0_0);
    }
    //点と点
    if(type == SmartDimension::distance){

        Pos t0 = target[0]->GetJointPos(0);
        Pos t1 = target[1]->GetJointPos(0);
        answer = new ArcRestraint(t0,(t1-t0).GetNormalize()*value);
    }
    //if(type == SmartDimension::)
    return answer;
}


SmartDimension::SmartDimension(){
}

