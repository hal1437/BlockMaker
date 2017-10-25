#include "CEdge.h"

double CEdge::GetDivisionRate(int divide,double grading,int count){
    if(divide == 1){
        if(count == 0)return 0;
        if(count == 1)return 1;
    }
    double A,B,sum=0,p,d;

    //エッジからpとLを抽出
    d = divide;
    p = grading;

    //指数関数パラメータ計算
    B = log(p) / (d-1);
    for(int i=1;i<=d;i++)sum += exp(B*i);
    A = 1 / sum;

    //指定番号までの総和
    double sum_rate=0;
    for(int i = 1;i <= count;i++){
        sum_rate += A*exp(B*i);
    }
    return sum_rate;
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
void CEdge::DrawArrow(double start,double end)const{
    //差分値
    Pos cc = (this->GetMiddleDivide(start) - this->GetMiddleDivide(end));

    //変換行列作成
    double theta1 = std::atan2(cc.y(),std::sqrt(cc.x()*cc.x()+cc.z()*cc.z()));
    double theta2 = std::atan2(-cc.x(),cc.z());
    Quat quat = Quat::getRotateXMatrix(theta1).Dot(Quat::getRotateYMatrix(theta2));

    //矢印作成準備
    double length = cc.Length();
    Pos p = this->GetMiddleDivide(end);// + cc * (-10);
    double ARROW_ROUND = length/8; //長さ
    const int ANGLE_COUNT = 16;
    glBegin(GL_POLYGON);
    glVertex3f(this->GetMiddleDivide(start).x(),
               this->GetMiddleDivide(start).y(),
               this->GetMiddleDivide(start).z());
    for(int i=0;i<=ANGLE_COUNT;i++){
        double a = 2*M_PI / ANGLE_COUNT * i;
        Pos c = Pos(ARROW_ROUND * std::sin(a),ARROW_ROUND * std::cos(a),0).Dot(quat);
        Pos pp = p+c;
        glVertex3f(pp.x(),pp.y(),pp.z());
    }
    glEnd();
}

void CEdge::DrawGL(Pos,Pos)const{
    if(!this->isVisible())return;
/*
    if(this->start != nullptr && this->end != nullptr && *this->start != *this->end){
        DrawArrow(0.90,0.89);
    }*/
    //分割ライン表示
    if(this->isVisibleDetail() && this->getDivide() > 0){
        for(double i = 1;i<this->getDivide();i++){
            Pos p  = GetDivisionPoint(i);
            Pos cc = GetDivisionPoint(i) - GetDivisionPoint(i+1);
            double theta1 = std::atan2(cc.y(),std::sqrt(cc.x()*cc.x()+cc.z()*cc.z()));
            double theta2 = std::atan2(-cc.x(),cc.z());
            double length = (*this->end - *this->start).Length();
            glBegin(GL_POLYGON);
            double ARROW_ROUND = length/100;
            for(double j=0;j<2*M_PI;j+= M_PI*2/3){
                Pos c = Pos(ARROW_ROUND*std::sin(j),ARROW_ROUND*std::cos(j),0).Dot(Quat::getRotateXMatrix(theta1).Dot(Quat::getRotateYMatrix(theta2)));
                Pos pp = p+c;
                glVertex3f(pp.x(),pp.y(),pp.z());
            }
            glEnd();
        }
    }
}
Pos CEdge::GetDivisionPoint(int count)const{
    Pos ans = this->GetMiddleDivide(GetDivisionRate(this->getDivide(),this->getGrading(),count));
    return ans;
}
void CEdge::SetStartPos(CObject* obj){
    this->SetChild(0,obj);
}

void CEdge::SetEndPos(CObject* obj){
    this->SetChild(this->GetChildCount()-1,obj);
}

CEdge::CEdge(QObject* parent):
    CObject(parent)
{
    this->divide  = 1;
    this->grading = 1.0;
    this->start = this->end = nullptr;
    this->SetVisibleDetail(false);
}
CEdge::CEdge(CPoint* start,CPoint* end,QObject* parent):
    CEdge(parent)
{
    this->start = start;
    this->end = end;
}

CEdge::~CEdge(){}

void CEdge::ChangeChildCallback(QVector<CObject*>){
    emit Changed(this);
}

