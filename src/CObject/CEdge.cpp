#include "CEdge.h"

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
void CEdge::DrawGL(Pos camera,Pos center)const{
    if(!this->isVisible())return;
    glBegin(GL_LINE_STRIP);
    //線の分割描画
    for(double i=0;i<=1;i += 1.0/CEdge::LINE_NEAR_DIVIDE){
        if(i+1.0/CEdge::LINE_NEAR_DIVIDE > 1)i=1;
        glVertex3f(this->GetMiddleDivide(i).x(),
                   this->GetMiddleDivide(i).y(),
                   this->GetMiddleDivide(i).z());
    }
    glEnd();

    if(this->start != nullptr && this->end != nullptr && *this->start != *this->end){
        //矢印
        Pos cc = (this->GetMiddleDivide(0.90) - this->GetMiddleDivide(0.89));
        double theta1 = std::atan2(cc.y(),std::sqrt(cc.x()*cc.x()+cc.z()*cc.z()));
        double theta2 = std::atan2(-cc.x(),cc.z());
        double length = (camera- center).Length();
        glBegin(GL_POLYGON);
        Pos p = this->GetMiddleDivide(0.91)+cc*(-10);
        glVertex3f(this->GetMiddleDivide(0.9).x(),
                   this->GetMiddleDivide(0.9).y(),
                   this->GetMiddleDivide(0.9).z());
        int ARROW_ROUND = length*10;
        for(double i=0;i<2*M_PI;i+= M_PI/32){
            Pos c = Pos(ARROW_ROUND*std::sin(i),ARROW_ROUND*std::cos(i),0).Dot(Quat::getRotateXMatrix(theta1).Dot(Quat::getRotateYMatrix(theta2)));
            Pos pp = p+c;
            glVertex3f(pp.x(),
                       pp.y(),
                       pp.z());
        }
        glEnd();
    }
    //分割ライン表示
    if(this->divide > 0){
        for(double i = 1;i<this->divide;i++){
            Pos p  = GetDivisionPoint(i);
            qDebug() << i << p;
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
    double A,B,sum=0,p,d,L;

    //エッジからpとLを抽出
    d = this->divide;
    p = this->grading;
    L = (*this->end - *this->start).Length();

    //指数関数パラメータ計算
    B = log(p) / (d-1);
    for(int i=1;i<=d;i++)sum += exp(B*i);
    A = L / sum;

    //指定番号までの総和
    double sum_rate=0;
    for(int i = 1;i <= count;i++){
        sum_rate += A*exp(B*i);
    }
    Pos ans = this->GetMiddleDivide(sum_rate/L);
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
    this->start = this->end = nullptr;
}

CEdge::~CEdge(){}
//点移動コールバック
void CEdge::ChangeChildCallback(CObject*){
    //何もしない
}

