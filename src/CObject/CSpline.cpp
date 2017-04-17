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

CSpline::CSpline()
{

}

CSpline::~CSpline()
{

}

bool CSpline::Refresh(){
    std::vector<double> x,y;
    for(int i=0;i<static_cast<int>(this->pos.size());i++){
        x.push_back(GetJointPos(i).x);
        y.push_back(GetJointPos(i).y);
    }
    xs.init(x);
    ys.init(y);
    return true;
}
bool CSpline::Create(CPoint* pos, int index){
    if(0 <= index){
        this->pos.push_back(pos);
        Refresh();
        return false;
    }
    return true;
}

Pos CSpline::GetNear(const Pos& )const{
    return Pos();// ちょっと解決策が浮かばない
}

void CSpline::Lock(bool lock){
    for(int i =0;i<static_cast<int>(this->pos.size());i++){
        pos[i]->Lock(lock);
    }
    CObject::Lock(lock);
}

bool CSpline::Draw(QPainter& painter)const{
    if(pos.size() > 1){
        QPainterPath path;
        double t, m;
        m = (double)(pos.size()-1);

        path.moveTo(GetJointPos(0).x,GetJointPos(0).y);
        const double dt = 1.0/DIVISION;
        for(t=0; t<=m; t += dt){
            if(t + dt > m)t=m;
            QPointF pos(xs.culc(t), ys.culc(t));
            path.lineTo(pos);
        }
        painter.setBrush(QColor(0,0,0,0));
        painter.drawPath(path);

    }
    for(int i=0;i<this->GetJointNum();i++){
        this->pos[i]->Draw(painter);
    }
    return true;
}
bool CSpline::isSelectable()const{
    double t, m;
    m = (double)(pos.size()-1);

    const double dt = 1.0/(DIVISION);
    for(t=0; t<=m; t += dt){
        if(t + dt > m)t=m;
        if((Pos(xs.culc(t),ys.culc(t)) - CObject::mouse_pos).Length() < COLLISION_SIZE){
            return true;
        }
    }
    return false;
}

bool CSpline::Move(const Pos& diff){
    for(int i=0;i<static_cast<int>(pos.size());i++){
        pos[i]->Move(diff);
    }
    return true;
}

int CSpline::GetJointNum()const{
    return this->pos.size();
}
Pos CSpline::GetJointPos(int index)const{
    return *pos[index];
}
CPoint* CSpline::GetJoint(int index){
    return pos[index];
}
void    CSpline::SetJoint(int index,CPoint* ptr){
    this->pos[index] = ptr;
}

