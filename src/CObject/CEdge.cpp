#include "CEdge.h"

void CEdge::SetStartPos(CPoint* pos){
    if(this->start != nullptr){
        disconnect(this->start,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
    }
    this->start = pos;
    connect   (this->start,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
}

void CEdge::SetEndPos(CPoint* pos){
    if(this->end != nullptr){
        disconnect(this->end,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
    }
    this->end = pos;
    connect(this->end,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
}

int CEdge::GetPosSequenceCount()const{
    return this->GetMiddleCount()+2;
}
CPoint* CEdge::GetPosSequence(int index)const{
    if     (index == 0                        )return this->start;
    else if(index == GetPosSequenceCount() - 1)return this->end;
    else if(index > 0 && index < GetPosSequenceCount() - 1)return this->GetMiddle(index-1);
    return nullptr;
}

Pos CEdge::GetNearLine(const Pos& pos1,const Pos& pos2)const{
    //GetNearPosによる近似
    Pos ans = *this->start;
    for(double i=0;i<1;i += 1.0/LINE_NEAR_DIVIDE){
        //より線に近い方を保存
        ans = std::max(this->GetMiddleDivide(i),ans,[&](Pos p1,Pos p2){
            return Pos::LineNearPoint(pos1,pos2,p1) < Pos::LineNearPoint(pos1,pos2,p2);
        });
    }
    return ans;
}
bool CEdge::DrawGL(Pos,Pos)const{
    if(!this->isVisible())return true;
    glBegin(GL_LINE_STRIP);
    //線の分割描画
    for(double i=0;i<=1;i += 1.0/CEdge::LINE_NEAR_DIVIDE){
        if(i+1.0/CEdge::LINE_NEAR_DIVIDE > 1)i=1;
        glVertex3f(this->GetMiddleDivide(i).x(),
                   this->GetMiddleDivide(i).y(),
                   this->GetMiddleDivide(i).z());
    }
    glEnd();

    //矢印
    Pos cc = (this->GetMiddleDivide(0.81) - this->GetMiddleDivide(0.8));
    double theta1 = std::atan2(cc.y(),std::sqrt(cc.x()*cc.x()+cc.z()*cc.z()));
    double theta2 = std::atan2(-cc.x(),cc.z());
    glBegin(GL_POLYGON);
    const int ARROW_ROUND = 10;
    Pos p = this->GetMiddleDivide(0.8);
    glVertex3f(this->GetMiddleDivide(0.9).x(),
               this->GetMiddleDivide(0.9).y(),
               this->GetMiddleDivide(0.9).z());
    for(double i=0;i<2*M_PI;i+= M_PI/32){
        Pos c = Pos(ARROW_ROUND*std::sin(i),ARROW_ROUND*std::cos(i),0).Dot(Quat::getRotateXMatrix(theta1).Dot(Quat::getRotateYMatrix(theta2)));
        Pos pp = p+c;
        glVertex3f(pp.x(),
                   pp.y(),
                   pp.z());
    }
    glEnd();

    return true;
}





CEdge::CEdge(QObject* parent):
    CObject(parent)
{
    this->start = this->end = nullptr;
}

CEdge::~CEdge()
{
}
CEdge::CEdge(CPoint* start,CPoint* end,QObject* parent):
    CEdge(parent){
    this->SetStartPos(start);
    this->SetEndPos(end);
}


//点移動コールバック
void CEdge::ChangePosCallback(CPoint* ,Pos ){
}

