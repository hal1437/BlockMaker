
#include "CArc.h"

double CArc::GetRound()const{
    return this->round;
}


bool CArc::Create(CPoint *start, CPoint *end){
    this->start  = start;
    this->end    = end;
    this->center = new CPoint((*this->start - *this->end) / 2 + *this->end,this->parent());
    return true; //終了
}
bool CArc::Draw(QPainter& painter)const{
    Pos dir1 = (*this->end   - *this->center);
    Pos dir2 = (*this->start - *this->center);
    double angle1 = std::atan2(-dir1.y,dir1.x) * 180 / PI;
    double angle2 = 360 - angle1 + (std::atan2(-dir2.y,dir2.x)) * 180 / PI;

    QRectF rect(this->center->x - round,
                this->center->y - round,
                round*2,round*2);
    painter.drawArc (rect,angle1*16,Mod(angle2,(360))*16);

    return true;
}
bool CArc::Move(const Pos& diff){
    this->start ->Move(diff);
    this->end   ->Move(diff);
    this->center->Move(diff);
    return true;
}
void CArc::Lock(bool lock){
    //それぞれロック
    this->start ->Lock(lock);
    this->end   ->Lock(lock);
    this->center->Lock(lock);
    CObject::Lock(lock);
}
int CArc::GetMiddleCount()const{
    return 1;
}
CPoint* CArc::GetMiddle(int index){
    if(index == 0){
        return this->center;
    }else{
        return nullptr;
    }
}


Pos CArc::GetNear(const Pos& hand)const{
    return Pos::CircleNearPoint(*this->center,round,hand);
}

CArc::CArc(QObject* parent):
    CEdge(parent)
{
}

CArc::~CArc()
{
}

void CArc::ChangePosCallback(const Pos& new_pos,const Pos& old_pos){

    if(this->isCreating()){
        *this->center = (*this->start - *this->end) / 2 + *this->end;
    }else{
        round = (new_pos-*this->center).Length();

        if(old_pos == *this->start){
            *this->start = (*this->end - *center).GetNormalize() * round;
        }
        if(old_pos == *this->end){
            *this->end = (*this->start - *center).GetNormalize() * round;
        }
    }
}


