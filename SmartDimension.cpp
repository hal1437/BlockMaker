#include "SmartDimension.h"



void SmartDimension::DrawString(QPainter& painter,const Pos& pos,const QString& str,double angle)const{
    painter.save();
    QTransform trans;
    int bit = (90 <= angle && angle < 270) ? 1 : 0;
    trans = QTransform::fromTranslate(pos.x,pos.y);
    QFontMetrics fm = painter.fontMetrics();

    trans.rotate(angle + 180*bit);
    painter.setTransform(trans);
    painter.drawText(-fm.width(str)*bit,-2,str);
    painter.restore();
}

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
    else if(obj1->is<CLine >() && obj2->is<CLine >())return SmartDimension::concurrent;
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
    if (this->type == length){
        //矢印
        Pos diff_base((target[0]->GetJointPos(0)-target[0]->GetJointPos(1)).GetNormalize());
        QPointF diff_(QPointF(diff_base.x,diff_base.y)*QTransform().rotate(90));
        Pos diff = Pos(diff_.x(),diff_.y());
        Pos p0 = target[0]->GetJointPos(0);
        Pos p1 = target[0]->GetJointPos(1);
        Pos t0 = target[0]->GetJointPos(0)+diff*15;
        Pos t1 = target[0]->GetJointPos(1)+diff*15;

        painter.drawLine(t0.x,t0.y,t1.x,t1.y);
        painter.drawLine((p0+diff*20).x,(p0+diff*20).y,p0.x,p0.y);
        painter.drawLine((p1+diff*20).x,(p1+diff*20).y,p1.x,p1.y);
        this->DrawArrow(painter,t0,(t0-t1).GetNormalize(),3.0);
        this->DrawArrow(painter,t1,(t1-t0).GetNormalize(),3.0);
        this->DrawString(painter,(t1+t0)/2.0,QString::number(value),Pos::Angle(p1-p0,Pos(1,0)));
        //寸法の文字
        return true;
    }
    if (this->type == distance){
        //矢印
        Pos t0 = target[0]->GetJointPos(0);
        Pos t1 = target[1]->GetJointPos(0);

        painter.drawLine(t0.x,t0.y,t1.x,t1.y);
        this->DrawArrow(painter,t0,(t0-t1).GetNormalize(),3.0);
        this->DrawArrow(painter,t1,(t1-t0).GetNormalize(),3.0);
        this->DrawString(painter,(t1+t0)/2.0,QString::number(value),Pos::Angle(t1-t0,Pos(1,0)));

        //寸法の文字
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
        this->DrawArrow(painter,d_pos[0],(d_pos[0]-d_pos[1]).GetNormalize(),3.0);
        this->DrawArrow(painter,d_pos[1],(d_pos[1]-d_pos[0]).GetNormalize(),3.0);
        this->DrawArrow(painter,d_pos[1],(d_pos[1]-d_pos[0]).GetNormalize(),3.0);
        this->DrawString(painter,(d_pos[0]+d_pos[1])/2,QString::number(value),Pos::Angle(d_pos[0]-d_pos[1],Pos(1,0)));

        //線からはみ出たら補助線を引く
        if(d_pos[1].y < std::min(lines[0].y,lines[1].y) ||  std::max(lines[0].y,lines[1].y) < d_pos[1].y){
            Pos near;//近い方
            near = std::max(lines[0],lines[1],[&](Pos l,Pos r){return ((l-d_pos[1]).Length() < (r-d_pos[1]).Length());});
            painter.drawLine(near.x,near.y,d_pos[1].x,d_pos[1].y);
        }
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
        Pos dir(QPoint(pos[0].x-center.x,pos[0].y-center.y)*QTransform().rotate(angle));
        Pos dir_p = dir + dir.GetNormalize()*50;

        painter.drawLine(center.x,center.y,(center+dir_p).x,(center+dir_p).y);
        DrawArrow(painter,(center+dir),-dir,3.0);

        //寸法の文字
        DrawString(painter,(center+dir+dir.GetNormalize()*20),QString("R")+QString::number(value),Pos::Angle(dir,Pos(1,0)));
        return true;
    }

    if (this->type == concurrent){
        //矢印
        CLine* line1 = dynamic_cast<CLine*>(target[0]);
        CLine* line2 = dynamic_cast<CLine*>(target[1]);
        Pos line1_pos[2] = {line1->GetJointPos(0),line1->GetJointPos(1)};
        Pos line2_pos[2] = {line2->GetJointPos(0),line2->GetJointPos(1)};

        std::sort(line1_pos,line1_pos+2);
        std::sort(line2_pos,line2_pos+2);

        Pos line1_base = (line1_pos[1] - line1_pos[0]).GetNormalize();
        Pos line2_base = (line2_pos[1] - line2_pos[0]).GetNormalize();

        Pos line1_c = (line1_pos[1] + line1_pos[0])/2;
        Pos line2_c = (line2_pos[1] + line2_pos[0])/2;

        Pos line_center = (line1_c + line2_c)/2;
        Pos line1_cc = line1_base * line1_base.Dot(line_center-line1_pos[0]) + line1_pos[0];
        Pos line2_cc = line2_base * line2_base.Dot(line_center-line2_pos[0]) + line2_pos[0];
        Pos dir      = (line1_cc - line2_cc).GetNormalize();


        //補助線

        if((line1_cc - line1_pos[0]).Length() > (line1_pos[1] - line1_pos[0]).Length() ||
           (line1_cc - line1_pos[1]).Length() > (line1_pos[0] - line1_pos[1]).Length()){
            Pos near = std::min(line1_pos[0],line1_pos[1],[&](Pos lhs,Pos rhs){return ((line1_cc - lhs).Length()<(line1_cc - rhs).Length());});
            Pos ap = line1_cc + (line1_cc - near).GetNormalize()*10;
            painter.drawLine(near.x,near.y,ap.x,ap.y);
        }

        if((line2_cc - line2_pos[0]).Length() > (line2_pos[1] - line2_pos[0]).Length() ||
           (line2_cc - line2_pos[1]).Length() > (line2_pos[0] - line2_pos[1]).Length()){
            Pos near = std::min(line2_pos[0],line2_pos[1],[&](Pos lhs,Pos rhs){return ((line2_cc - lhs).Length()<(line2_cc - rhs).Length());});
            Pos ap = line2_cc + (line2_cc - near).GetNormalize()*10;
            painter.drawLine(near.x,near.y,ap.x,ap.y);
        }


        painter.drawLine(line1_cc.x,line1_cc.y,line2_cc.x,line2_cc.y);
        DrawArrow(painter,line1_cc, dir,3.0);
        DrawArrow(painter,line2_cc,-dir,3.0);
        //寸法の文字
        DrawString(painter,line_center,QString::number(value),Pos::Angle(dir,Pos(1,0)));
        return true;
    }

    return false;
}

std::vector<Restraint*> SmartDimension::MakeRestraint(){
    std::vector<Restraint*> answer;

    //点と直線
    if(type == SmartDimension::distanceLine){
        answer.push_back(new MatchRestraint({target[0],target[1]},value));
    }
    //点と点
    if(type == SmartDimension::distance){
        answer.push_back(new MatchRestraint({target[0],target[1]},value));
    }
    //線だけ
    if(type == SmartDimension::length){
        answer.push_back(new MatchRestraint({target[0]->GetJoint(0),target[0]->GetJoint(1)},value));
    }
    //円弧
    if(type == SmartDimension::radius){
        answer.push_back(new MatchRestraint({target[0]->GetJoint(-1),
                                             target[0]->GetJoint(0),
                                             target[0]->GetJoint(1)},value));
    }
    //並行
    if(type == SmartDimension::concurrent){
        answer.push_back(new MatchRestraint({target[0],target[1]->GetJoint(0)},value));
        answer.push_back(new MatchRestraint({target[0],target[1]->GetJoint(1)},value));
    }
    return answer;
}


SmartDimension::SmartDimension(){
}

