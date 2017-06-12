
#include "CArc.h"

double CArc::GetRound()const{
    return this->round;
}

CREATE_RESULT CArc::Create(CPoint *pos){
    if(this->center == nullptr){
        this->SetCenterPos(pos);
        this->start = this->center;
    }else if(this->start == this->center){
        this->SetStartPos(pos);
        this->end = this->start;
    }else if(this->end == this->start){
        this->SetEndPos(pos);
        //this->center = new CPoint((*this->start - *this->end) / 2 + *this->end,this->parent());
        connect(this->start ,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
        connect(this->end   ,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
        connect(this->center,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));

    }else{
        disconnect(this->start ,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
        disconnect(this->end   ,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
        disconnect(this->center,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
        this->center = pos;
        round = Pos(*this->end - *this->center).Length();
        connect(this->start ,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
        connect(this->end   ,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
        connect(this->center,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
    }
    return CREATE_RESULT::THREESHOT; //終了
}
bool CArc::Draw(QPainter& painter)const{
    if(this->start == nullptr && this->end == nullptr && this->center == nullptr)return false;
    QRectF rect(this->center->x() - round,
                this->center->y() - round,
                round*2,round*2);
    Pos dir1 = (*this->start   - *this->center);
    double angle1 = std::atan2(-dir1.y(),dir1.x()) * 180 / PI;

    if(this->end == this->start){
        //点線
        QPen pen = painter.pen();
        painter.save();
        pen.setStyle(Qt::DashLine);
        painter.setPen(pen);
        painter.drawChord(rect,angle1*16,360*16);
        painter.restore();
    }else{
        //実線
        Pos dir2 = (*this->end - *this->center);
        double angle1 = std::atan2(-dir1.y(),dir1.x()) * 180 / PI;
        double angle2 = 360 - angle1 + (std::atan2(-dir2.y(),dir2.x())) * 180 / PI;

        QRectF rect(this->center->x() - round,
                    this->center->y() - round,
                    round*2,round*2);
        painter.drawArc (rect,angle1*16,Mod(angle2,(360))*16);
    }

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
    return (Pos::Angle(*this->start - *this->center,*this->end - *this->center) >
            Pos::Angle(*this->start - *this->center,pos        - *this->center)) &&
           (std::abs((pos - *this->center).Length() - this->round) < COLLISION_SIZE);
}

//始点終点操作オーバーライド
void CArc::SetStartPos(CPoint* pos){
    if(this->start!=nullptr)ChangePosCallback(pos,*this->start);
    CEdge::SetStartPos(pos);
}

void CArc::SetEndPos(CPoint* pos){
    if(this->end!=nullptr)ChangePosCallback(pos,*this->end);
    CEdge::SetEndPos(pos);
}
void CArc::SetCenterPos(CPoint* pos){
    if(this->center != nullptr){
        disconnect(this->center,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
    }
    this->center = pos;
    connect   (this->center,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
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
        ChangePosCallback(pos,*this->center);
        disconnect(this->center,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
        this->center = pos;
        connect   (this->center,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
    }
}
Pos CArc::GetMiddleDivide(double t)const{
    double angle = Pos::Angle(*this->start-*this->center,*this->end-*this->center)*PI/180;
    qDebug() << angle*180/PI;
    Pos center_base = (*this->start-*this->center).Cross(*this->end-*this->center);
    Pos ans = Pos::RodriguesRotate(*this->start-*this->center,center_base,angle*t)+*this->center; //要検討
    return ans;
}

Pos CArc::GetNearPos (const Pos& hand)const{
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
    this->start = nullptr;
    this->end   = nullptr;
    this->center = nullptr;
}
CArc::CArc(CPoint* start,CPoint* end,QObject* parent):
    CEdge(parent){
    this->SetStartPos(start);
    this->SetEndPos(end);
}
CArc::~CArc()
{
}

void CArc::ChangePosCallback(CPoint *pos, Pos ){

    round = (*this->center - *this->start).Length();

    //中心の移動

    if(pos == this->center){
        if(this->start->isLock()){
            round = (*this->start - *this->center).Length();
            this->end->Move((*this->end - *this->center).GetNormalize() * round + *this->center - *this->end);
        }else if(this->end->isLock()){
            round = (*this->end - *this->center).Length();
            this->start->Move((*this->start - *this->center).GetNormalize() * round + *this->center - *this->start);
        }else{
            this->start->Move(this->GetNearPos(*this->start) - *this->start);
            this->end  ->Move(this->GetNearPos(*this->end)   - *this->end);
        }
    }else{
        round = (*pos-*this->center).Length();
    }
    //定義済みであれば
    if(this->end != nullptr){
        if(pos == this->start){
            if(this->end->isLock()){
                *this->center = (*this->start - *this->end) / 2 + *this->end;
            }else{
                this->end  ->Move((*this->end   - *this->center).GetNormalize() * round + *this->center - *this->end);
            }
        }
        if(pos == this->end){
            if(this->start->isLock()){
                *this->center = (*this->start - *this->end) / 2 + *this->end;
            }else{
                this->start->Move((*this->start - *this->center).GetNormalize() * round + *this->center - *this->start);
            }
        }
    }
}


