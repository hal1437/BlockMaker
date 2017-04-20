#include "CRect.h"

#include "CLine.h"

#include <iostream>

bool CRect::Create(CPoint *start, CPoint *end){
    pos[0] = start;
    pos[1] = new CPoint();
    pos[2] = new CPoint();
    pos[3] = end;
    //構築
    for(int i=0;i<4;i++)lines[i] = new CLine();
    *pos[1] = Pos(pos[0]->x,pos[3]->y);
    *pos[2] = Pos(pos[3]->x,pos[0]->y);
    lines[0]->Create(pos[0],pos[1]);
    lines[1]->Create(pos[0],pos[2]);
    lines[2]->Create(pos[2],pos[3]);
    lines[3]->Create(pos[1],pos[3]);
    return false;
}

Pos CRect::GetNear(const Pos& hand)const{
    return Pos::LineNearPoint(*this->pos[0],*this->pos[1],hand);
}
void CRect::Lock(bool lock){
    for(int i =0;i<4;i++){
        this->lines[i]->Lock(lock);
    }
    this->Lock(lock);
}

bool CRect::Draw(QPainter& painter)const{
    QPointF p1 = *pos[0];
    QPointF p2 = *pos[1];
    QPointF p3 = *pos[2];
    QPointF p4 = *pos[3];

    painter.drawLine(p1,p2);
    painter.drawLine(p1,p3);
    painter.drawLine(p2,p4);
    painter.drawLine(p3,p4);
    return true;
}

bool CRect::Move(const Pos& diff){
    for(int i=0;i<4;i++){
        this->pos[i]->Move(diff);
    }
    return true;
}

CLine* CRect::GetLines(int index){
    return lines[index];
}

CRect::CRect(QObject *parent):
    CObject(parent)
{
    for(int i=0;i<4;i++)this->lines[i] = nullptr;
}

CRect::~CRect()
{

}

