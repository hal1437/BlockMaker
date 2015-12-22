#include "CLine.h"

#include <iostream>

bool CLine::Create(Relative<Pos> pos, int index){
    if(0 <= index && index < 2){
        this->pos[index] = pos;
        if(index==1){
            this->is_Creating = false;
            return true;
        }else{
            this->is_Creating = true;
            return false;
        }
    }
    return false;
}

Pos CLine::GetNear(const Pos& hand)const{
    return Pos::LineNearPoint(pos[0](),pos[1](),hand);
}

bool CLine::Draw(QPainter& painter)const{
    if(this->selecting)painter.setPen(QPen(Qt::red , DRAWING_LINE_SIZE));
    else               painter.setPen(QPen(Qt::blue, DRAWING_LINE_SIZE));
    if(is_Creating){
        painter.drawLine(QPointF(pos[0].getRelative().x,pos[0].getRelative().y),
                         QPointF(this->mouse_over.x,this->mouse_over.y));
    }else{
        painter.drawLine(QPointF(pos[0].getRelative().x,pos[0].getRelative().y),
                         QPointF(pos[1].getRelative().x,pos[1].getRelative().y));
    }
    return true;
}
bool CLine::Selecting(){
    //直線と点の距離のアルゴリズム
    float d,d_;
    Pos v1,v2;
    v1 = pos[1].getRelative() - pos[0].getRelative();
    v2 = mouse_over           - pos[0].getRelative();
    d_ = (v1.x*v2.x + v1.y*v2.y) / (std::sqrt(std::pow(v2.x,2)+std::pow(v2.y,2))*std::sqrt(std::pow(v1.x,2)+std::pow(v1.y,2)));
    d  = std::sqrt(1-d_*d_) * std::sqrt(std::pow(v1.x,2)+std::pow(v1.y,2));
    if(d < COLLISION_SIZE && ((pos[0].getRelative().x < mouse_over.x && mouse_over.x < pos[1].getRelative().x) ||
                              (pos[1].getRelative().x < mouse_over.x && mouse_over.x < pos[0].getRelative().x) ||
                              (pos[0].getRelative().y < mouse_over.y && mouse_over.y < pos[1].getRelative().y) ||
                              (pos[1].getRelative().y < mouse_over.y && mouse_over.y < pos[0].getRelative().y))){
        selecting=true;
        return true;
    }else{
        selecting=false;
        return false;
    }
}

bool CLine::isLocked(){
    return false;
}
bool CLine::Move(const Pos& diff){
    for(int i = 0;i<2;i++){
        pos[i].getReferenceSame()->diff += diff;
    }
}



CLine::CLine()
{

}

CLine::~CLine()
{

}

