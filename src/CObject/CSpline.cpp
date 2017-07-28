#include "CSpline.h"



//スプラインデータ初期化
void Spline::init(std::vector<double> sp)
{
    double tmp, w[MaxSplineSize+1];
    int i;
    num = sp.size()-1;

    // ３次多項式の0次係数(a)を設定
    for(i=0; i<=num; i++) {
        a[i] = sp[i];
    }

    // ３次多項式の2次係数(c)を計算
    // 連立方程式を解く。
    // 但し、一般解法でなくスプライン計算にチューニングした方法
    c[0] = c[num] = 0.0;
    for(i=1; i<num; i++){
        c[i] = 3.0 * (a[i-1] - 2.0 * a[i] + a[i+1]);
    }
    // 左下を消す
    w[0]=0.0;
    for(i=1; i<num; i++) {
        tmp = 4.0 - w[i-1];
        c[i] = (c[i] - c[i-1])/tmp;
        w[i] = 1.0 / tmp;
    }
    // 右上を消す
    for(i=num-1; i>0; i--) {
        c[i] = c[i] - c[i+1] * w[i];
    }

    // ３次多項式の1次係数(b)と3次係数(b)を計算
    b[num] = d[num] =0.0;
    for(i=0; i<num; i++) {
        d[i] = ( c[i+1] - c[i]) / 3.0;
        b[i] = a[i+1] - a[i] - c[i] - d[i];
    }
}

//媒介変数(0～num-1の実数）に対する値を計算
double Spline::culc(double t)const
{
    int j;
    double dt;
    j = (int)floor(t); // 小数点以下切捨て
    if(j < 0) j=0;
    else if (j >= num) j=num-1; // 丸め誤差を考慮

    dt = t - (double)j;
    return a[j] + ( b[j] + (c[j] + d[j] * dt) * dt ) * dt;
}


CREATE_RESULT CSpline::Create(CPoint *pos){
    connect(pos,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
    if(this->start == nullptr){
        this->start = pos;
    }else if(this->end == nullptr){
        this->end = pos;
    }else{
        //endを更新
        this->pos.push_back(end);
        this->end = pos;
        RefreshNodes();

    }
    return CREATE_RESULT::ENDLESS;
}

//中間点操作
CPoint*  CSpline::GetPoint(int index){
    if(index == 0)return this->start;
    if(index > 0 && index < static_cast<int>(this->pos.size()) - 2)return this->pos[index-1];
    if(index == static_cast<int>(this->pos.size())-1)return this->end;
    return nullptr;
}
CObject* CSpline::GetChild(int index){
    return this->GetPoint(index);
}
void     CSpline::SetChild(int index,CObject* obj){
    IgnoreChild(this->GetChild(index));
    if(index == 0)this->start = dynamic_cast<CPoint*>(obj);
    if(index > 0 && index < static_cast<int>(this->pos.size())-2)this->pos[index-1] = dynamic_cast<CPoint*>(obj);
    if(index == static_cast<int>(this->pos.size())-1)this->end = dynamic_cast<CPoint*>(obj);
    ObserveChild(this->GetChild(index));
}

int CSpline::GetChildCount()const{
    return this->pos.size()+2;
}
Pos CSpline::GetMiddleDivide(double t)const{
    if(pos.size() == 0)return (*end - *start)*t + *start;
    double tt = t * (pos.size()+1);
    return Pos(xs.culc(tt),ys.culc(tt),zs.culc(tt));
}

Pos CSpline::GetNearPos(const Pos& pos)const{
    if(this->pos.size() >= 1){
        QVector<Pos> pp;
        const double dt = 1.0/DIVISION;
        double t, m;
        m = (double)(this->pos.size()+1);
        for(t=0; t<=m; t += dt){
            if(t + dt > m)t=m;
            pp.push_back(Pos(xs.culc(t), ys.culc(t),zs.culc(t)));
        }
        //もっとも近いもの
        return *std::min_element(pp.begin(),pp.end(),[&](Pos lhs,Pos rhs){
            return ((pos-lhs).Length() < (pos-rhs).Length());
        });
    }else{
        //いつもの直線のアレ
        return Pos::LineNearPoint(*this->start,*this->end,pos);
    }
}

void CSpline::RefreshNodes(){
    std::vector<double> x,y,z;
    if(this->start != nullptr){
        x.push_back(this->start->x());
        y.push_back(this->start->y());
        z.push_back(this->start->z());
    }
    for(CPoint* p : this->pos){
        x.push_back(p->x());
        y.push_back(p->y());
        z.push_back(p->z());
    }
    if(this->end != nullptr){
        x.push_back(this->end->x());
        y.push_back(this->end->y());
        z.push_back(this->end->z());
    }
    xs.init(x);
    ys.init(y);
    zs.init(z);
}

CSpline::CSpline(QObject *parent):
    CEdge(parent)
{
}
CSpline::CSpline(CPoint* start,CPoint* end,QObject* parent):
    CEdge(parent){
    this->SetChild(0,start);
    this->SetChild(1,end);
}

CEdge* CSpline::Clone()const{
    CSpline* ptr = new CSpline(this->parent());
    ptr->start   = new CPoint(*this->start ,ptr);
    ptr->end     = new CPoint(*this->end   ,ptr);
    for(int i=0;i<this->GetChildCount();i++){
        ptr->pos.push_back(new CPoint(*dynamic_cast<CPoint*>(dynamic_cast<const CObject*>(this)->GetChild(i)),ptr));
    }
    return ptr;
}

CSpline::~CSpline()
{

}
//点移動コールバックオーバーライド
void CSpline::ChangePosCallback(CPoint *, Pos old_pos){
    CPoint* p;
    if(*this->start == old_pos) p = this->start;
    if(*this->end   == old_pos) p = this->end;
    for(int i = 0;i < this->GetChildCount();i++){
        if(*dynamic_cast<CPoint*>(this->GetChild(i)) == old_pos){
            p = dynamic_cast<CPoint*>(this->GetChild(i));
        }
    }
    //一時的にnew_posを適用
//    p = new_pos;
    RefreshNodes();
//    p = old_pos;
}


