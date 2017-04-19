#include "CLine.h"

bool CLine::Create(CPoint *pos, int index){
    if(index==0){
        this->start = pos;
        return false;//継続
    }else if(index==1){
        this->end = pos;
        return true; //終了
    }
    return false;
}
bool CLine::Draw(QPainter& painter)const{
    //描画
    painter.drawLine(*this->start,*this->end);
    return true;
}
bool CLine::Move(const Pos& diff){
    //ジョイントそれぞれを動かす
    if(!this->start->isLock())this->start->Move(diff);
    if(!this->end  ->isLock())this->end  ->Move(diff);
    return true;
}
void CLine::Lock(bool lock){
    //それぞれロック
    this->start->Lock(lock);
    this->end  ->Lock(lock);
    //自分もロック
    CObject::Lock(lock);
}


//中間点操作
int CLine::GetMiddleCount()const{
    return 0;
}
CPoint* CLine::GetMiddle(int){
    return nullptr;
}


Pos CLine::GetNear(const Pos& hand)const{
    //点と直線の最近点
    return Pos::LineNearPoint(*this->start,*this->end,hand);
}

CLine::CLine(QObject *parent):
    CEdge(parent)
{
}

CLine::~CLine()
{
}

