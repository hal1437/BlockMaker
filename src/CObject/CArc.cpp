
#include "CArc.h"


double CArc::GetRound()const{
    return this->round;
}

CREATE_RESULT CArc::Create(CPoint *pos){
    CREATE_RESULT result = COMPLETE;
    if(this->center == nullptr){
        this->SetCenterPos(pos);
        this->start = nullptr;
        result = CREATE_RESULT::TWOSHOT;
    }else if(this->start == nullptr){
        this->SetStartPos(pos);
        this->end = nullptr;
        result = CREATE_RESULT::ONESHOT;
    }else if(this->end == nullptr){
        this->SetEndPos(pos);
        result = CREATE_RESULT::COMPLETE;
    }else{
        this->center = pos;
        round = Pos(*this->end - *this->center).Length();
         result = CREATE_RESULT::COMPLETE;
    }
    return result; //終了
}
void CArc::DrawGL(Pos camera,Pos center)const{
    if(!this->isVisible())return;
    if(this->end == nullptr){
        glBegin(GL_LINES);
        Pos cc = camera - center;
        double theta1 = std::atan2(cc.y(),std::sqrt(cc.x()*cc.x()+cc.z()*cc.z()));
        double theta2 = std::atan2(-cc.x(),cc.z());
        //点線円の描画
        for(double k=0;k < 2*M_PI;k += M_PI/32){
            const int length = (*this->start-*this->center).Length();
            Pos p = Pos(length*std::sin(k),length*std::cos(k),0).Dot(Quat::getRotateXMatrix(theta1).Dot(Quat::getRotateYMatrix(theta2)));
            glVertex3f((p + *this->center).x(),(p + *this->center).y(),(p + *this->center).z());
        }
        glEnd();
    }else{
        glBegin(GL_LINE_STRIP);
        //線の分割描画
        for(double i=0;i<=1;i += 1.0/CArc::LINE_NEAR_DIVIDE){
            if(i+1.0/CArc::LINE_NEAR_DIVIDE > 1)i=1;
            glVertex3f(this->GetMiddleDivide(i).x(),
                       this->GetMiddleDivide(i).y(),
                       this->GetMiddleDivide(i).z());
        }
        glEnd();
        CEdge::DrawGL(camera,center);
    }
}
bool CArc::isSelectable(Pos pos) const{
    //角度判定、半径判定、作成判定
    return (Pos::Angle(*this->start - *this->center,*this->end - *this->center) >
            Pos::Angle(*this->start - *this->center,pos        - *this->center)) &&
           (std::abs((pos - *this->center).Length() - this->round) < COLLISION_SIZE);
}

CPoint*  CArc::GetPoint(int index){
    if(index == 0)return this->start;
    if(index == 1)return this->center;
    if(index == 2)return this->end;
    return nullptr;
}

CObject* CArc::GetChild     (int index){
    return this->GetPoint(index);
}
void     CArc::SetChild(int index,CObject* obj){
    IgnoreChild(this->GetChild(index));
    if(index == 0)this->start  = dynamic_cast<CPoint*>(obj);
    if(index == 1)this->center = dynamic_cast<CPoint*>(obj);
    if(index == 2)this->end    = dynamic_cast<CPoint*>(obj);
    ObserveChild(this->GetChild(index));
}

int CArc::GetChildCount()const{
    return 3;
}
Pos CArc::GetMiddleDivide(double t)const{
    if(this->start == nullptr || this->end == nullptr)return *this->center;
    Pos center_base = (*this->start-*this->center).Cross(*this->end-*this->center);
    Pos ans;

    if(this->isReverse()==false){
        double angle = Pos::Angle(*this->start-*this->center,*this->end-*this->center)*PI/180;
        ans = Pos::RodriguesRotate(*this->start-*this->center,center_base,angle*t)+*this->center; //要検討
    } else {
        double angle = Pos::Angle(*this->start-*this->center,*this->end-*this->center)*PI/180;
        ans = Pos::RodriguesRotate(*this->end-*this->center,center_base,(2*PI-angle)*t)+*this->center; //要検討
    }
    return ans;
}

void CArc::SetCenterPos(CPoint *pos){
    this->SetChild(1,pos);
}

Pos CArc::GetNearPos (const Pos& hand)const{
    return Pos::CircleNearPoint(*this->center,round,hand);
}

CEdge* CArc::Clone()const{
    CArc* ptr   = new CArc(this->parent());
    ptr->start  = new CPoint(*this->start ,ptr);
    ptr->end    = new CPoint(*this->end   ,ptr);
    ptr->center = new CPoint(*this->center,ptr);
    ptr->grading = this->grading;
    ptr->divide  = this->divide;
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

void CArc::ChangeChildCallback(QVector<CObject*> child){
    if(this->start==nullptr || this->end==nullptr)return;

    //中心の移動
    if(exist(child,this->center)){
        //平均値
        round = ((*this->start - *this->center).Length() + (*this->end - *this->center).Length())/2;
        this->start->MoveAbsolute((*this->start - *this->center).GetNormalize() * round + *this->center);
        this->end  ->MoveAbsolute((*this->end   - *this->center).GetNormalize() * round + *this->center);
    }
    //始点
    if(exist(child,this->start)){
        round = (*this->start - *this->center).Length();
        this->end->MoveAbsolute((*this->end   - *this->center).GetNormalize() * round + *this->center);
    }
    //終点
    if(exist(child,this->end)){
        round = (*this->end - *this->center).Length();
        this->start->MoveAbsolute((*this->start - *this->center).GetNormalize() * round + *this->center);
    }
    emit Changed(this);
}


