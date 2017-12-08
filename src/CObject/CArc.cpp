
#include "CArc.h"

CREATE_RESULT CArc::Create(CPoint *pos){
    CREATE_RESULT result = COMPLETE;
    if(this->center == nullptr){
        this->SetCenterPos(pos);
        this->start = nullptr;
        result = CREATE_RESULT::TWOSHOT;
    }else if(this->start == nullptr){
        this->SetStartPos(pos);
        this->end = nullptr;
        round_s = (*this->start - *this->center).Length();
        result = CREATE_RESULT::ONESHOT;
    }else if(this->end == nullptr){
        this->SetEndPos(pos);
        round_e = (*this->end   - *this->center).Length();
        result = CREATE_RESULT::COMPLETE;
    }else{
        // ???
        result = CREATE_RESULT::COMPLETE;
    }
    return result; //終了
}
void CArc::DrawGL(Pos camera,Pos center)const{
    if(!this->isVisible())return;
    if(this->end == nullptr){
        //通常描画
        Pos cc = camera - center;
        double theta1 = std::atan2(cc.y(),std::sqrt(cc.x()*cc.x()+cc.z()*cc.z()));
        double theta2 = std::atan2(-cc.x(),cc.z());
        Quat q = Quat::getRotateXMatrix(theta1).Dot(Quat::getRotateYMatrix(theta2));//変換行列
        //点線円の描画
        glBegin(GL_LINES);
        for(double k=0;k < 2*M_PI;k += M_PI/32){
            const int length = (*this->start-*this->center).Length();
            Pos p = Pos(length*std::sin(k),length*std::cos(k),0).Dot(q);
            glVertex3f((p + *this->center).x(),(p + *this->center).y(),(p + *this->center).z());
        }
        glEnd();
    }else{

        //conflict
        if(std::abs(this->round_s - this->round_e) > SAME_ANGLE_EPS){

            glColor3f(1,0,0);
            //ずれの分を点線で描画
            Pos p1 = this->GetMiddleDivide(0);
            Pos p2 = this->GetMiddleDivide(1);
            //始点
            glBegin(GL_LINES);
            for(double i=0;i<=1;i += 1.0/CArc::LINE_NEAR_DIVIDE/2){
                if(i+1.0/CArc::LINE_NEAR_DIVIDE > 1)i=1;
                Pos pp = (*this->start - p1) * i + p1;
                glVertex3f(pp.x(),pp.y(),pp.z());
            }
            glEnd();
            //終点
            glBegin(GL_LINES);
            for(double i=0;i<=1;i += 1.0/CArc::LINE_NEAR_DIVIDE/2){
                if(i+1.0/CArc::LINE_NEAR_DIVIDE > 1)i=1;
                Pos pp = (*this->end - p2) * i + p2;
                glVertex3f(pp.x(),pp.y(),pp.z());
            }
            glEnd();
        }
        glBegin(GL_LINE_STRIP);
        //円弧の分割描画
        for(double i=0;i<=1;i += 1.0/CArc::LINE_NEAR_DIVIDE){
            if(i+1.0/CArc::LINE_NEAR_DIVIDE > 1)i=1;
            glVertex3f(this->GetMiddleDivide(i).x(),
                       this->GetMiddleDivide(i).y(),
                       this->GetMiddleDivide(i).z());
        }
        glEnd();

        DrawArrow(0.90,0.80);
        CEdge::DrawGL(camera,center);
    }
}
bool CArc::isSelectable(Pos pos) const{
    //角度判定、半径判定、作成判定
    double dd = (pos - *this->center).Length();
    return (Pos::Angle(*this->start - *this->center,*this->end - *this->center) >
            Pos::Angle(*this->start - *this->center,pos        - *this->center)) &&
           (std::abs(dd - this->round_s) < COLLISION_SIZE ||
            std::abs(dd - this->round_e) < COLLISION_SIZE );
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
    this->ChangeChildCallback({obj});
    ObserveChild(this->GetChild(index));
}

int CArc::GetChildCount()const{
    return 3;
}
Pos CArc::GetMiddleDivide(double t)const{
    if(this->start == nullptr || this->end == nullptr)return *this->center;
    Pos center_base = (*this->start-*this->center).Cross(*this->end-*this->center);
    Pos ans;
    double round = (this->round_e + this->round_s)/2;

    if(this->isReverse()==false){
        double angle = Pos::Angle(*this->start-*this->center,*this->end-*this->center)*PI/180;
        ans = Pos::RodriguesRotate((*this->start-*this->center).GetNormalize() * round,center_base,angle*t)+*this->center; //要検討
    } else {
        double angle = Pos::Angle(*this->start-*this->center,*this->end-*this->center)*PI/180;
        ans = Pos::RodriguesRotate((*this->end-*this->center).GetNormalize() * round,center_base,(2*PI-angle)*t)+*this->center; //要検討
    }
    return ans;
}

void CArc::SetCenterPos(CPoint *pos){
    this->SetChild(1,pos);
}

Pos CArc::GetNearPos (const Pos& hand)const{
    Pos p1 = Pos::CircleNearPoint(*this->center,round_s,hand);
    Pos p2 = Pos::CircleNearPoint(*this->center,round_e,hand);
    return (p1 - hand).Length() > (p2 - hand).Length() ? p2 : p1;
}


CEdge* CArc::Clone()const{
    CArc* ptr   = new CArc(this->parent());
    ptr->Create(new CPoint(*this->center ,ptr));
    ptr->Create(new CPoint(*this->start  ,ptr));
    ptr->Create(new CPoint(*this->end    ,ptr));
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

    //保持点ならば同一半形状の動きのみ行う
    if(exist(child,this->start) && CPoint::hanged == this->start && *this->start != *this->center){
        this->start->MoveAbsolute((*this->start   - *this->center).GetNormalize() * round_s + *this->center);
    }
    if(exist(child,this->end) && CPoint::hanged == this->end && *this->end   != *this->center){
        this->end->MoveAbsolute((*this->end   - *this->center).GetNormalize() * round_e + *this->center);
    }
    if(*this->start != *this->center)round_s = (*this->start - *this->center).Length();
    if(*this->end   != *this->center)round_e = (*this->end   - *this->center).Length();
    emit Changed(this);
}


