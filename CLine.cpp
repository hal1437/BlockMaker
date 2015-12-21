#include "CLine.h"

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

bool CLine::Draw(QPainter& painter)const{
    painter.setPen(QPen(Qt::blue, 2));
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
    return false;
}

bool CLine::isLocked(){
    return false;
}


CLine::CLine()
{

}

CLine::~CLine()
{

}

