#include "CFace.h"

#define SWITCHING_STRING_TO_BOUNDARY_TYPE(TYPE,COMP)\
if(COMP == #TYPE)return Boundary::Type::TYPE;

#define SWITCHING_BOUNDARY_TYPE_TO_STRING(TYPE,COMP)\
if(COMP == Boundary::Type::TYPE) return #TYPE;\

Boundary::Type Boundary::StringToBoundaryType(QString str){
    SWITCHING_STRING_TO_BOUNDARY_TYPE(patch,str)
    SWITCHING_STRING_TO_BOUNDARY_TYPE(wall,str)
    SWITCHING_STRING_TO_BOUNDARY_TYPE(symmetryPlane,str)
    SWITCHING_STRING_TO_BOUNDARY_TYPE(cyclic,str)
    SWITCHING_STRING_TO_BOUNDARY_TYPE(cyclicAMI,str)
    SWITCHING_STRING_TO_BOUNDARY_TYPE(wedge,str)
    SWITCHING_STRING_TO_BOUNDARY_TYPE(none,str)
    return Boundary::Type::none;
}
QString Boundary::BoundaryTypeToString(Boundary::Type type){
    SWITCHING_BOUNDARY_TYPE_TO_STRING(patch        ,type)
    SWITCHING_BOUNDARY_TYPE_TO_STRING(wall         ,type)
    SWITCHING_BOUNDARY_TYPE_TO_STRING(symmetryPlane,type)
    SWITCHING_BOUNDARY_TYPE_TO_STRING(cyclic       ,type)
    SWITCHING_BOUNDARY_TYPE_TO_STRING(cyclicAMI    ,type)
    SWITCHING_BOUNDARY_TYPE_TO_STRING(wedge        ,type)
    SWITCHING_BOUNDARY_TYPE_TO_STRING(none         ,type)
    return "none";
}

CFace* CFace::base[3];

bool CFace::Creatable(QVector<CObject*> lines){
    if(std::any_of(lines.begin(),lines.end(),[](CObject* p){return !p->is<CEdge>();}))return false;
    if(lines.size() < 3)return false;

    //閉じた図形テスト
    std::map<CPoint*,int> point_maps;
    QVector<CPoint*> points;
    for(CObject* line: lines){
        point_maps[dynamic_cast<CEdge*>(line)->start]++;
        points.push_back(dynamic_cast<CEdge*>(line)->start);
        point_maps[dynamic_cast<CEdge*>(line)->end]++;
        points.push_back(dynamic_cast<CEdge*>(line)->end);
    }
    //全ての点の数が2である
    if(!std::all_of(point_maps.begin(),point_maps.end(),[](std::pair<CObject*,int> c){return c.second==2;}))return false;

    //重複削除
    std::sort(points.begin(),points.end());
    points.erase(std::unique(points.begin(),points.end()),points.end());

    return true;
}

bool CFace::isComprehension(Pos pos)const{
    if(this->edges.size()<3)return true;
    else {
        //法線ベクトルとの内積が0であれば平面上に存在する。
        return this->GetNorm().DotPos(pos-*this->GetPointSequence(0)) < 1.0e-10;
    }
}

Pos CFace::GetNorm()const{
    Pos vec1,vec2;
    CPoint *p1,*p2;
    p1 = this->GetPointSequence(0);
    p2 = this->GetPointSequence(1);

    vec1 = *p2 - *p1;
    vec2 = *this->GetPointSequence(this->edges.size()-1) - *this->GetPointSequence(0);

    if(this->edges.size() < 2 || (vec1-vec2).Length()<SAME_POINT_EPS)return Pos();
    else{
        return Pos(vec1.Cross(vec2)).GetNormalize();
    }
}
Pos CFace::GetNorm(double u,double v)const{
    const double DELTA = 0.0001;
    Pos vec1,vec2;
    vec1 = this->GetPosFromUV(u + DELTA,v) - this->GetPosFromUV(u,v);
    vec2 = this->GetPosFromUV(u,v + DELTA) - this->GetPosFromUV(u,v);

    if(this->edges.size() < 2)return Pos();
    else{
        return Pos(vec1.Cross(vec2)).GetNormalize();
    }
}

Pos CFace::GetPosFromUV(double u,double v)const{
    //全ての曲線の差分値の合計
    Pos delta[4];
    //曲線の反対側の座標を計算原点をセット
    delta[0] = this->GetPosFromUVSquare(  u,1.0);
    delta[1] = this->GetPosFromUVSquare(  0,  v);
    delta[2] = this->GetPosFromUVSquare(  u,  0);
    delta[3] = this->GetPosFromUVSquare(1.0,  v);
    //曲面に対して垂直方向の座標値を加算
    delta[0] += (this->GetEdge(0)->GetMiddleDivide(u) - delta[0]) * (1.0-v);
    delta[1] += (this->GetEdge(1)->GetMiddleDivide(v) - delta[1]) * u;
    delta[2] += (this->GetEdge(2)->GetMiddleDivide(1.0-u) - delta[2]) * v;
    delta[3] += (this->GetEdge(3)->GetMiddleDivide(1.0-v) - delta[3]) * (1.0-u);
    //GetPosFromUVSquareとの差分を取得
    for(int i=0;i<4;i++){
        delta[i] -= this->GetPosFromUVSquare(u,v);
    }

    return this->GetPosFromUVSquare(u,v) + (delta[0]+delta[1]+delta[2]+delta[3]);
}
Pos CFace::GetPosFromUVSquare(double u,double v)const{
    //双1次曲面の式
    //S(u,v)=(1-u)(1-v)P_{0}+u(1-v)P_{1}+(1-u)vP_{2}+uvP_{3};
    Pos p[4] = {*this->GetEdge(0)->start,
                *this->GetEdge(1)->start,
                *this->GetEdge(2)->start,
                *this->GetEdge(3)->start};
    return (p[0] * (1-u) * (1-v) +
            p[1] *     u * (1-v) +
            p[3] * (1-u) * v +
            p[2] *     u * v);
}
CPoint* CFace::GetBasePoint()const{
    QVector<CPoint*> pp;
    for(CEdge* edge:this->edges){
        pp.push_back(edge->start);
        pp.push_back(edge->end);
    }
    unique(pp);

    //左下の探索
    CPoint* corner = nullptr;//左下
    double LIMIT_LENGTH = 0;
    for(CPoint* pos:pp){
        QVector<double> vs = {pos->x(),pos->y(),pos->z(),LIMIT_LENGTH};
        LIMIT_LENGTH = *std::max_element(vs.begin(),vs.end());
    }
    //角の算出
    Pos limit(-LIMIT_LENGTH,-LIMIT_LENGTH,-LIMIT_LENGTH);//最果て
    corner =  *std::min_element(pp.begin(),pp.end(),[&](CPoint* lhs,CPoint* rhs){
        return ((*lhs-limit).Length() < (*rhs-limit).Length());
    });
    return corner;
}
CEdge*  CFace::GetBaseEdge()const{
    CPoint* base = this->GetBasePoint();

    //基準点を含むエッジ
    QVector<CEdge*> ee;
    for(CEdge* e :this->edges){
        if(e->start == base || e->end == base){
            ee.push_back(e);
        }
    }
    //X軸方向長さが最も大きいもの
    Pos n = Pos(1,0,0);
    return *std::max_element(ee.begin(),ee.end(),[&](CEdge* lhs,CEdge* rhs){
        return (*lhs->end - *lhs->start).DotPos(n) >
               (*rhs->end - *rhs->start).DotPos(n);
    });
}
CPoint* CFace::GetPointSequence(int index)const{
    //index回だけ連鎖させる
    CPoint* corner = this->GetBasePoint();
    CPoint* ans = corner;
    CPoint* old = corner; //反復連鎖防止
    QVector<CPoint*> candidate;//連鎖候補
    for(int i=0;i<index%this->edges.size();i++){
        //ansを含むlineを探す
        for(CEdge* line:this->edges){
            if(ans == line->start && old != line->end){
                candidate.push_back(line->end);
            }else if(ans == line->end && old != line->start){
                candidate.push_back(line->start);
            }
        }
        //選定
        old = ans;
        if(candidate.size() == 2){
            //二択
            if(Pos::Angle(*candidate[0]-*corner,*candidate[1]-*corner) > Pos::Angle(*candidate[1]-*corner,*candidate[0]-*corner)){
                ans = candidate[0];
            }else{
                ans = candidate[1];
            }
        }else if(candidate.size() == 1){
            ans = candidate[0];
        }else{
            ans = corner;
        }
        candidate.clear();
    }
    if(ans == nullptr){
        qDebug() << "CFace::GetPointSequence(" << index << ") error. answer is nullptr.";
    }
    return ans;
}
CEdge*  CFace::GetEdgeSequence(int index)const{
    //index番目の点とindex+1番目の点を含む点を持つエッジを探す
    QVector<CEdge*>::const_iterator it = std::find_if(this->edges.begin(),this->edges.end(),[&](CEdge* edge){
        CPoint*  p1 = this->GetPointSequence(index);
        CPoint*  p2 = this->GetPointSequence((index+1)%this->edges.size());
        return (edge->start == p1 && edge->end == p2) ||
               (edge->start == p2 && edge->end == p1);
    });
    if(it == this->edges.end()){
        //?
        CPoint* pp[4];
        for(int i =0;i<4;i++)pp[i] = this->GetPointSequence(i);
        qDebug() << "?";
        return nullptr;
    }

    //反転
    CEdge* ans = *it;
    /*
    if(ans->end == this->GetPointSequence(index)){
        std::swap(ans->start,ans->end);
    }*/
    return ans;
}
void CFace::DrawGL(Pos,Pos)const{
    if(!this->isVisible())return;
    if(this->isPolygon()){
        //薄い色に変更
        float currentColor[4];
        glGetFloatv(GL_CURRENT_COLOR,currentColor);
        glColor4f(currentColor[0],currentColor[1],currentColor[2], 0.1);
        glDepthMask(GL_FALSE);

        //塗りつぶし描画
        const double FACE_DIVIDE = 10.0;//面分割数
        for (int j = 0; j < FACE_DIVIDE; j++){
          for (int i = 0; i < FACE_DIVIDE; i++){
              glBegin(GL_QUADS);

              Pos pp[4] = {
                  this->GetPosFromUV(i    /FACE_DIVIDE , j/FACE_DIVIDE),
                  this->GetPosFromUV(i    /FACE_DIVIDE , (j+1)/FACE_DIVIDE),
                  this->GetPosFromUV((i+1)/FACE_DIVIDE , (j+1)/FACE_DIVIDE),
                  this->GetPosFromUV((i+1)/FACE_DIVIDE , j/FACE_DIVIDE)
              };
              for(int k=0;k<4;k++){
                  glVertex3f(pp[k].x(),pp[k].y(),pp[k].z());
              }
              glEnd();
          }
        }
        glEnd();
        glDepthMask(GL_TRUE);

        //色を復元
        glColor4f(currentColor[0],currentColor[1],currentColor[2], currentColor[3]);

        //メッシュ分割描画
        int u_max = std::min(this->GetEdge(0)->divide,this->GetEdge(2)->divide);//u方向分割
        int v_max = std::min(this->GetEdge(1)->divide,this->GetEdge(3)->divide);//v方向分割
        if(u_max < 0)u_max = 1;
        if(v_max < 0)v_max = 1;
        for(double i=0;i<u_max;i++){//u方向ループ
            for(double j=0;j<v_max;j++){//v方向ループ
                //倍率計算
                double rate_0 = CEdge::GetDivisionRate(u_max,  this->GetEdge(0)->grading,i);
                double rate_2 = CEdge::GetDivisionRate(u_max,1/this->GetEdge(2)->grading,i);
                double rate_1 = CEdge::GetDivisionRate(v_max,  this->GetEdge(1)->grading,j);
                double rate_3 = CEdge::GetDivisionRate(v_max,1/this->GetEdge(3)->grading,j);

                double rate_p1 = (rate_2-rate_0) * (  j  /u_max) + rate_0;
                double rate_p3 = (rate_2-rate_0) * ((j+1)/u_max) + rate_0;
                double rate_p2 = (rate_1-rate_3) * (  i  /v_max) + rate_3;
                double rate_p4 = (rate_1-rate_3) * ((i+1)/v_max) + rate_3 ;
                //座標計算
                Pos p[] = {this->GetPosFromUV(rate_p1    ,j/v_max),
                           this->GetPosFromUV(rate_p3    ,(j+1)/v_max),
                           this->GetPosFromUV(i/u_max    ,rate_p2),
                           this->GetPosFromUV((i+1)/u_max,rate_p4)};
                //描画
                glBegin(GL_LINES);
                for(int k=0;k<4;k++){
                    if(j == 0 && k >= 2)continue;//線上は描画しない
                    if(i == 0 && k  < 2)continue;//線上は描画しない
                    glVertex3f(p[k].x(),p[k].y(),p[k].z());
                }
                glEnd();
            }
        }
    }else{

        //外枠
        glBegin(GL_LINE_LOOP);
        for(int i=0;i<this->edges.size();i++){
            glVertex3f(this->GetPointSequence(i)->x(),this->GetPointSequence(i)->y(), this->GetPointSequence(i)->z());
        }
        glEnd();
    }

}
bool CFace::DrawNormArrowGL()const{
    Pos center;
    for(int i=0;i<this->edges.size();i++){
        center += *this->GetPointSequence(i);
    }
    center /= this->edges.size();

    glBegin(GL_LINES);
    glVertex3f(center.x(),center.y(), center.z());
    glVertex3f((center+this->GetNorm()*100).x(),(center+this->GetNorm()*100).y(), (center+this->GetNorm()*100).z());
    glEnd();
    return true;
}

void CFace::ReorderEdges(){
    QVector<CEdge*> ans;
    for(int i=0;i<this->edges.size();i++){
        ans.push_back(this->GetEdgeSequence(i));
    }
    this->edges = ans;
}

//子の操作
CEdge*  CFace::GetEdge      (int index){
    return this->edges[index];
}
CEdge*  CFace::GetEdge      (int index)const{
    return this->edges[index];
}
CObject* CFace::GetChild     (int index){
    return this->GetEdge(index);
}
void CFace::SetChild(int index,CObject* obj){
    if(this->GetChildCount() <= index){
        this->edges.resize(index);
    }
    this->edges[index] = dynamic_cast<CEdge*>(obj);
}

int      CFace::GetChildCount()const{
    return this->edges.size();
}
Pos      CFace::GetEdgeMiddle(int index,double t)const{
    bool reverse = false;
    if(this->reorder.size() > index){
        reverse = this->reorder[index];
    }
    if(reverse)return this->GetEdge(index)->GetMiddleDivide(1.0 - t);
    else       return this->GetEdge(index)->GetMiddleDivide(t);
}

Pos CFace::GetNearPos (const Pos&)const{
    return Pos();
}
Pos CFace::GetNearLine(const Pos& ,const Pos& )const{
    return Pos();
}

CObject* CFace::Clone()const{
    CFace* new_obj = new CFace();
    for(CEdge* edge:this->edges){
        new_obj->edges.push_back(dynamic_cast<CEdge*>(edge->Clone()));
    }
    //構成点マージ
    for(int i=0;i<new_obj->GetChildCount();i++){
        for(int j=0;j<new_obj->GetChildCount();j++){
            if(*new_obj->GetEdge(i)->start == *new_obj->GetEdge(j)->start)new_obj->GetEdge(j)->start = new_obj->GetEdge(i)->start;
            if(*new_obj->GetEdge(i)->start == *new_obj->GetEdge(j)->end  )new_obj->GetEdge(j)->end   = new_obj->GetEdge(i)->start;
            if(*new_obj->GetEdge(i)->end   == *new_obj->GetEdge(j)->start)new_obj->GetEdge(j)->start = new_obj->GetEdge(i)->end;
            if(*new_obj->GetEdge(i)->end   == *new_obj->GetEdge(j)->end  )new_obj->GetEdge(j)->end   = new_obj->GetEdge(i)->end;
        }
    }
    new_obj->name     = this->name;
    new_obj->boundary = this->boundary;
    return new_obj;
}

CFace::CFace(QObject* parent):
    CObject(parent)
{
}

CFace::~CFace()
{
}

