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

void CSpline::Lock(bool lock){
    for(int i =0;i<static_cast<int>(this->pos.size());i++){
        pos[i]->Lock(lock);
    }
    CObject::Lock(lock);
}

bool CSpline::Draw(QPainter& painter)const{
    if(!this->isVisible())return true;
    if(pos.size() >= 1){
        QPainterPath path;
        double t, m;
        m = (double)(pos.size()+1);

        path.moveTo(this->start->x(),this->start->y());
        const double dt = 1.0/DIVISION;
        for(t=0; t<=m; t += dt){
            if(t + dt > m)t=m;
            QPointF pos(xs.culc(t), ys.culc(t));
            path.lineTo(pos);
        }
        painter.setBrush(QColor(0,0,0,0));
        painter.drawPath(path);
    }else{
        painter.drawLine(QPointF(this->start->x(),this->start->y()),QPointF(this->end->x(),this->end->y()));
    }
    return true;
}

bool CSpline::Move(const Pos& diff){
    this->start->Move(diff);
    this->end  ->Move(diff);
    for(int i=0;i<static_cast<int>(pos.size());i++){
        pos[i]->Move(diff);
    }
    return true;
}

//始点終点操作オーバーライド
void CSpline::SetStartPos(CPoint* pos){
    CEdge::SetStartPos(pos);
    RefreshNodes();
}

void CSpline::SetEndPos(CPoint* pos){
    CEdge::SetEndPos(pos);
    RefreshNodes();
}

//中間点操作
int CSpline::GetMiddleCount()const{
    return this->pos.size();
}
CPoint* CSpline::GetMiddle(int index)const{
    return pos[index];
}
void CSpline::SetMiddle(CPoint* pos,int index){
    this->pos[index] = pos;
}
Pos  CSpline::GetMiddleDivide(double t)const{
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
    x.push_back(this->start->x());
    y.push_back(this->start->y());
    z.push_back(this->start->z());
    for(CPoint* p : this->pos){
        x.push_back(p->x());
        y.push_back(p->y());
        z.push_back(p->z());
    }
    x.push_back(this->end->x());
    y.push_back(this->end->y());
    z.push_back(this->end->z());
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
    this->SetStartPos(start);
    this->SetEndPos(end);
}

CEdge* CSpline::Clone()const{
    CSpline* ptr = new CSpline(this->parent());
    ptr->start   = new CPoint(*this->start ,ptr);
    ptr->end     = new CPoint(*this->end   ,ptr);
    for(int i=0;i<this->GetMiddleCount();i++){
        ptr->pos.push_back(new CPoint(*this->GetMiddle(i),ptr));
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
    for(int i = 0;i < this->GetMiddleCount();i++){
        if(*this->GetMiddle(i) == old_pos){
            p = this->GetMiddle(i);
        }
    }
    //一時的にnew_posを適用
//    p = new_pos;
    RefreshNodes();
//    p = old_pos;
}


