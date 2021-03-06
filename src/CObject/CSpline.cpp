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
    this->ObserveChild(pos);
    if(this->start == nullptr){
        this->start = pos;
    }else if(this->end == nullptr){
        this->end = pos;
    }else{
        //endを更新
        this->pos.push_back(end);
        this->end = pos;
        Refresh();

    }
    return CREATE_RESULT::ENDLESS;
}
void CSpline::DrawGL(Pos camera,Pos center)const{
    glBegin(GL_LINE_STRIP);
    //線の分割描画
    for(double i=0;i<=1;i += 1.0/(CSpline::LINE_NEAR_DIVIDE*(this->pos.size()+1))){
        if(i+1.0/(CSpline::LINE_NEAR_DIVIDE*(this->pos.size()+1)) > 1)i=1;
        glVertex3f(this->GetMiddleDivide(i).x(),
                   this->GetMiddleDivide(i).y(),
                   this->GetMiddleDivide(i).z());
    }
    glEnd();

    //矢印の描画
    const double ARROW_DIVIDE_RATE = 0.9;
    const double ARROW_LENGTH_RATE = 0.1;
    for(int i=0;i<this->pos.size()+1;i++){
        double begin = 1.0 / (this->pos.size()+1) * i;
        double end   = 1.0 / (this->pos.size()+1) * (i+1);
        DrawArrow(begin+(end-begin)*ARROW_DIVIDE_RATE,
                  begin+(end-begin)*(ARROW_DIVIDE_RATE-ARROW_LENGTH_RATE));
    }
    CEdge::DrawGL(camera,center);
}


//中間点操作
CPoint*  CSpline::GetPoint(int index){
    if     (index <= 0                 )return this->start;
    else if(index <= this->pos.size()  )return this->pos[index-1];
    else if(index >= this->pos.size()+1)return this->end;
    return nullptr;
}
CPoint*  CSpline::GetPoint(int index)const{
    if     (index <= 0                 )return this->start;
    else if(index <= this->pos.size()  )return this->pos[index-1];
    else if(index >= this->pos.size()+1)return this->end;
    return nullptr;
}
CObject* CSpline::GetChild(int index){
    return GetPoint(index);
}
CObject* CSpline::GetChild(int index)const{
    return GetPoint(index);
}
void     CSpline::SetChild(int index,CObject* obj){

    if(obj == nullptr){
        //シグナル全解除
        for(int i = 0;i< this->GetChildCount();i++){
            IgnoreChild(this->GetChild(i));
        }
        //切り詰め処理
        if(index <= 0){
            if(this->GetChildCount()==1){
                //全て削除された状態
                this->start = nullptr;
            }else{
                this->start = this->pos.front();
                this->pos.erase(this->pos.begin());//切り詰め
            }
        }
        else if(index <= this->pos.size()){
            this->pos.erase(std::next(this->pos.begin(),index-1));//切り詰め
        }
        else if(index >= this->pos.size()+1){
            this->end = this->pos.back();
            this->pos.pop_back();
        }
        //シグナル再接続
        for(int i = 0;i< this->GetChildCount();i++){
            ObserveChild(this->GetChild(i));
        }
    }else{
        //入れ替え処理
        IgnoreChild(this->GetChild(index));
        if     (index <= 0                 )this->start        = dynamic_cast<CPoint*>(obj);
        else if(index <= this->pos.size()  )this->pos[index-1] = dynamic_cast<CPoint*>(obj);
        else if(index >= this->pos.size()+1)this->end          = dynamic_cast<CPoint*>(obj);
        ObserveChild(this->GetChild(index));
    }
}

int CSpline::GetChildCount()const{
    int sum =0;
    if(this->start != nullptr)sum++;
    if(this->end   != nullptr)sum++;
    sum += this->pos.size();
    return sum;
}
Pos CSpline::GetMiddleDivide(double t)const{
    if(pos.size() == 0)return (*end - *start)*t + *start;
    double tt = t * (pos.size()+1);
    return Pos(xs.culc(tt),ys.culc(tt),zs.culc(tt));
}

Pos CSpline::GetNearPos(const Pos& pos)const{
    //制御点ごとの区間を三分探索
    QList<double> dd;
    auto L = [&](double t){
        return (this->GetMiddleDivide(t) - pos).Length();
    };
    //区間ごとに
    for(int i=0;i<=this->pos.size();i++){
        double p = MinimumSearch(   i /(this->pos.size()+1.0),
                                 (i+1)/(this->pos.size()+1.0),
                                 L);
        dd.push_back(p);
    }

    //Lが最小になる値を求める
    Pos p = this->GetMiddleDivide(*std::min_element(dd.begin(),dd.end(),[&](double lhs,double rhs){
        return L(lhs) < L(rhs);
    }));
    return p;
}

Pos CSpline::GetNearLine(const Pos& pos1,const Pos& pos2)const{
    //制御点ごとの区間を三分探索
    QList<double> dd;
    Pos dir = (pos1 - pos2).GetNormalize();   //投影方向

    //距離関数
    auto L = [&](double t){
        return (this->GetMiddleDivide(t).Projection(dir) - pos1).Length();
    };

    //区間ごとに距離関数の最小値を集める
    for(int i=0;i<=this->pos.size();i++){
        double p = MinimumSearch(   i /(this->pos.size()+1.0),
                                 (i+1)/(this->pos.size()+1.0),
                                 L);
        dd.push_back(p);
    }

    //Lが最小になる値を求める
    Pos p = this->GetMiddleDivide(*std::min_element(dd.begin(),dd.end(),[&](double lhs,double rhs){
        return L(lhs) < L(rhs);
    }));
    return  p;
}

void CSpline::Refresh(){
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

//結合
CEdge*          CSpline::MergeEdge (CEdge* merge){
    //型が同じでない
    if(!merge->is<CSpline>()){
        QMessageBox::critical(nullptr, tr("結合エラー"), tr("型が違います。同一タイプの線を選択してください。"));
        return nullptr;
    }
    //一致しない
    CPoint* merge_point;
    if(this->start == merge->start)merge_point = this->start;
    if(this->start == merge->end  )merge_point = this->start;
    if(this->end   == merge->start)merge_point = this->end;
    if(this->end   == merge->end  )merge_point = this->end;
    if(merge_point == nullptr){
        QMessageBox::critical(nullptr, tr("結合エラー"), tr("二つの線の始点または終点が一致している必要があります。"));
        return nullptr;
    }

    //作成
    CSpline* new_line = new CSpline();
    if(this->end == merge_point){
        for(int i=0;i<this->GetChildCount()-1;i++){
            new_line->Create(dynamic_cast<CPoint*>(this->GetChild(i)));
        }
    }else{
        for(int i=this->GetChildCount()-1;i>=1;i--){
            new_line->Create(dynamic_cast<CPoint*>(this->GetChild(i)));
        }
    }
    if(merge->end == merge_point){
        for(int i=merge->GetChildCount()-1;i>=0;i--){
            new_line->Create(dynamic_cast<CPoint*>(merge->GetChild(i)));
        }
    }else{
        for(int i=0;i<merge->GetChildCount();i++){
            new_line->Create(dynamic_cast<CPoint*>(merge->GetChild(i)));
        }
    }
    return new_line;
}
QList<CEdge*> CSpline::DivideEdge(CPoint* division){
    //補間値を取得
    double middle_param = this->GetMiddleParamFromPos(*division);

    CEdge* edge_s = new CSpline();
    CEdge* edge_e = new CSpline();
    for(int i =0;i<middle_param * (this->GetChildCount()-1);i++){
        edge_s->Create(dynamic_cast<CPoint*>(this->GetChild(i)));
    }
    edge_s->Create(division);
    edge_e->Create(division);
    for(int i = middle_param * (this->GetChildCount()-1) + 1;i<this->GetChildCount();i++){
        edge_e->Create(dynamic_cast<CPoint*>(this->GetChild(i)));
    }
    return {edge_s,edge_e};
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
    for(int i= 0;i<this->GetChildCount();i++){
        ptr->Create(dynamic_cast<CPoint*>(this->GetChild(i)->Clone()));
    }
    ptr->grading = this->grading;
    ptr->divide  = this->divide;
    ptr->Refresh();
    return ptr;
}

CSpline::~CSpline()
{

}
//点移動コールバックオーバーライド
void CSpline::ChangeChildCallback(QList<CObject *> ){
    Refresh();
}


