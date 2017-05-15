
#include "CArc.h"

double CArc::GetRound()const{
    return this->round;
}


CREATE_RESULT CArc::Create(CPoint *pos){
    if(this->start == nullptr){
        this->start  = pos;
    }else if(this->end == nullptr){
        this->end    = pos;
        this->center = new CPoint((*this->start - *this->end) / 2 + *this->end,this->parent());
        connect(this->start ,SIGNAL(PosChanged(Pos,Pos)),this,SLOT(ChangePosCallback(Pos,Pos)));
        connect(this->end   ,SIGNAL(PosChanged(Pos,Pos)),this,SLOT(ChangePosCallback(Pos,Pos)));
        connect(this->center,SIGNAL(PosChanged(Pos,Pos)),this,SLOT(ChangePosCallback(Pos,Pos)));

    }else{
        disconnect(this->start ,SIGNAL(PosChanged(Pos,Pos)),this,SLOT(ChangePosCallback(Pos,Pos)));
        disconnect(this->end   ,SIGNAL(PosChanged(Pos,Pos)),this,SLOT(ChangePosCallback(Pos,Pos)));
        disconnect(this->center,SIGNAL(PosChanged(Pos,Pos)),this,SLOT(ChangePosCallback(Pos,Pos)));
        this->center = pos;
        round = (*this->end - *this->center).Length();
        connect(this->start ,SIGNAL(PosChanged(Pos,Pos)),this,SLOT(ChangePosCallback(Pos,Pos)));
        connect(this->end   ,SIGNAL(PosChanged(Pos,Pos)),this,SLOT(ChangePosCallback(Pos,Pos)));
        connect(this->center,SIGNAL(PosChanged(Pos,Pos)),this,SLOT(ChangePosCallback(Pos,Pos)));
    }
    return CREATE_RESULT::TWOSHOT; //終了
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
bool CArc::isSelectable(Pos pos) const{
    //角度判定、半径判定、作成判定
    return (Pos::Angle(*this->start - *this->center,*this->end - *this->center) <
            Pos::Angle(*this->start - *this->center,pos        - *this->center)) &&
           (std::abs((pos - *this->center).Length() - this->round) < COLLISION_SIZE);
}

//始点終点操作オーバーライド
void CArc::SetStartPos(CPoint* pos){
    ChangePosCallback(*pos,*this->start);
    CEdge::SetStartPos(pos);
}

void CArc::SetEndPos(CPoint* pos){
    ChangePosCallback(*pos,*this->end);
    CEdge::SetEndPos(pos);
}

int CArc::GetMiddleCount()const{
    return 1;
}
CPoint* CArc::GetMiddle(int index)const{
    if(index == 0){
        return this->center;
    }else{
        return nullptr;
    }
}
void CArc::SetMiddle(CPoint* pos,int index){
    if(index == 0){
        ChangePosCallback(*pos,*this->center);
        disconnect(this->center,SIGNAL(PosChanged(Pos,Pos)),this,SLOT(ChangePosCallback(Pos,Pos)));
        this->center = pos;
        connect(this->center,SIGNAL(PosChanged(Pos,Pos)),this,SLOT(ChangePosCallback(Pos,Pos)));
    }
}


Pos CArc::GetNear(const Pos& hand)const{
    return Pos::CircleNearPoint(*this->center,round,hand);
}
CEdge* CArc::Clone()const{
    CArc* ptr   = new CArc(this->parent());
    ptr->start  = new CPoint(*this->start ,ptr);
    ptr->end    = new CPoint(*this->end   ,ptr);
    ptr->center = new CPoint(*this->center,ptr);
    return ptr;
}

CArc::CArc(QObject* parent):
    CEdge(parent)
{
}
CArc::~CArc()
{
}

void CArc::ChangePosCallback(const Pos& new_pos,const Pos& old_pos){

    //中心の移動
    if(old_pos == *this->center){
        *this->center = new_pos; //中心座標を一時的に次位置へ
        if(this->start->isLock()){
            round = (*this->start - *this->center).Length();
            this->end->Move((*this->end - *this->center).GetNormalize() * round + *this->center - *this->end);
        }else if(this->end->isLock()){
            round = (*this->end - *this->center).Length();
            this->start->Move((*this->start - *this->center).GetNormalize() * round + *this->center - *this->start);
        }else{
            this->start->Move(this->GetNear(*this->start) - *this->start);
            this->end  ->Move(this->GetNear(*this->end)   - *this->end);
        }
        *this->center = old_pos; //中心座標を復元
    }else{
        round = (new_pos-*this->center).Length();
    }
    if(old_pos == *this->start){
        if(this->end->isControlPoint() || this->end->isLock()){
            *this->center = (*this->start - *this->end) / 2 + *this->end;
        }else{
            this->end  ->Move((*this->end   - *this->center).GetNormalize() * round + *this->center - *this->end);
        }
    }
    if(old_pos == *this->end){
        if(this->start->isControlPoint() || this->start->isLock()){
            *this->center = (*this->start - *this->end) / 2 + *this->end;
        }else{
            this->start->Move((*this->start - *this->center).GetNormalize() * round + *this->center - *this->start);
        }
    }
}


