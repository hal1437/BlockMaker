#include "CEdge.h"


CEdge::Grading CEdge::Grading::GetReverse()const{
    CEdge::Grading ans = *this;
    std::reverse(ans.elements.begin(),ans.elements.end());
    for(GradingElement& elm:ans.elements){
        elm.grading = 1.0/elm.grading; //反転
    }
    return ans;
}


double CEdge::GetDivisionRate(int divide,Grading grading,int count){
    if(divide == 1){
        if(count == 0)return 0;
        if(count == 1)return 1;
    }
    if(divide == count)return 1;

    //部分問題始点と終点を探す
    double min = 0,max = 0,cc = count;
    int count_begin = 0;
    int i;
    for(i = 0;;i++){
        cc  -= (divide *grading[i].cell ); // カウント減少
        if(cc < 0){

            max = min + (grading[i].dir);
            break;
        }
        min += grading[i].dir;   //方向割合増加
        count_begin += divide * (grading[i].cell);
    }

    //min~maxを0~1と見て通常の分割寄せのアレをやる
    double A,B,sum=0,p,d;

    //設定からpとLを抽出
    d = divide * grading[i].cell;
    p = grading[i].grading;

    //指数関数パラメータ計算
    B = log(p) / (d-1);
    for(int i=1;i<=d;i++)sum += exp(B*i);
    A = 1 / sum;

    //指定番号までの総和
    double sum_rate=0;
    for(int i = 1;i <= (count-count_begin);i++){
        sum_rate += A*exp(B*i);
    }
    return sum_rate * (max-min) + min;
}

bool CEdge::isOnEdge(const Pos& hand)const{
    return (this->GetNearPos(hand) - hand).Length() < CObject::SAME_POINT_EPS;
}

Pos CEdge::GetNearPos(const Pos& pos)const{
    //距離関数を三分探索
    double p = MinimumSearch(0.0,1.0,[&](double t){
        return (this->GetMiddleDivide(t) - pos).Length();
    });

    //Lが最小になる値を求める
    return this->GetMiddleDivide(p);
}

Pos CEdge::GetNearLine(const Pos& pos1,const Pos& pos2)const{
    //pos1とpos2の直線で投影
    Pos dir = (pos1 - pos2).GetNormalize();   //投影方向

    //距離関数L(t)
    double p = MinimumSearch(0.0,1.0,[&](double t){
        Pos pos_t     = this->GetMiddleDivide(t);       //tの点
        Pos pos_t_pro = pos_t - dir * dir.DotPos(pos_t); //tの点を投影
        return (pos_t_pro - pos1).Length();
    });

    //Lが最小になる値を求める
    return this->GetMiddleDivide(p);
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
    }
    */
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

Pos    CEdge::GetDifferentialVec   (Pos pos)const{
    const double DIFF_DELTA = 0.001;//差分
    double t = this->GetMiddleParamFromPos(pos);
    if     (t + DIFF_DELTA > 1.0)return (this->GetMiddleDivide(1.0)        - this->GetMiddleDivide(1.0-DIFF_DELTA)).GetNormalize(); //後退差分
    else if(t - DIFF_DELTA < 0.0)return (this->GetMiddleDivide(DIFF_DELTA) - this->GetMiddleDivide(0.0)           ).GetNormalize(); //前方差分
    else {
        //中央差分
        return (this->GetMiddleDivide(t + DIFF_DELTA) - this->GetMiddleDivide(t - DIFF_DELTA)).GetNormalize();
    }
}
double CEdge::GetMiddleParamFromPos(Pos pos)const{
    //もっともPosに近く点を三分探索で求める。
    return MinimumSearch(0.0,1.0,[&](double t){
        return (this->GetMiddleDivide(t) - pos).Length();
    });
}
//結合
CEdge*          CEdge::MergeEdge (CEdge* merge){
}
QVector<CEdge*> CEdge::DivideEdge(CPoint* division){
    return     QVector<CEdge*>();
}

CEdge::CEdge(QObject* parent):
    CObject(parent)
{
    this->divide  = 1;
    this->grading.elements.push_back({1.0,1.0,1.0});
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

