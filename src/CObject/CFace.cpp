#include "CFace.h"

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

    //同一平面テスト
    /*
    Pos cross = (*points[1] - *points[0]).Cross
                (*points[2] - *points[0]);
    for(int i=3;i<point_maps.size();i++){
        double d = (*points[i] - *points[0]).DotPos(cross);
        if(!NearlyEqual(d,0)){
            return false;
        }
    }*/
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
void CFace::DefineMap2()const{
    CEdge* ee[]={this->GetEdgeSequence(0),
                 this->GetEdgeSequence(1),
                 this->GetEdgeSequence(2),
                 this->GetEdgeSequence(3)};
    if(std::any_of(ee,ee+4,[](CEdge* edge){
        return edge == nullptr;
    }))return;
    //二次元エバリュエータ
    GLfloat ctrlpoints[4][4][3];
    for(int i=0;i<3;i++){
        ctrlpoints[0][0][i] = ee[0]->GetMiddleDivide(0).mat[i];
        ctrlpoints[1][0][i] = ee[0]->GetMiddleDivide(1.0/3.0).mat[i];
        ctrlpoints[2][0][i] = ee[0]->GetMiddleDivide(2.0/3.0).mat[i];
        ctrlpoints[3][0][i] = ee[1]->GetMiddleDivide(0).mat[i];
        ctrlpoints[0][1][i] = ee[3]->GetMiddleDivide(2.0/3.0).mat[i];
        ctrlpoints[3][1][i] = ee[1]->GetMiddleDivide(1.0/3.0).mat[i];
        ctrlpoints[0][2][i] = ee[3]->GetMiddleDivide(1.0/3.0).mat[i];
        ctrlpoints[3][2][i] = ee[1]->GetMiddleDivide(2.0/3.0).mat[i];
        ctrlpoints[0][3][i] = ee[3]->GetMiddleDivide(0).mat[i];
        ctrlpoints[1][3][i] = ee[2]->GetMiddleDivide(2.0/3.0).mat[i];
        ctrlpoints[2][3][i] = ee[2]->GetMiddleDivide(1.0/3.0).mat[i];
        ctrlpoints[3][3][i] = ee[2]->GetMiddleDivide(0).mat[i];

        ctrlpoints[1][1][i] = (ctrlpoints[1][0][i] - ctrlpoints[0][0][i]) + (ctrlpoints[0][1][i] - ctrlpoints[0][0][i]) +  ctrlpoints[0][0][i];
        ctrlpoints[2][1][i] = (ctrlpoints[2][0][i] - ctrlpoints[3][0][i]) + (ctrlpoints[3][1][i] - ctrlpoints[3][0][i]) +  ctrlpoints[3][0][i];
        ctrlpoints[1][2][i] = (ctrlpoints[0][2][i] - ctrlpoints[0][3][i]) + (ctrlpoints[1][3][i] - ctrlpoints[0][3][i]) +  ctrlpoints[0][3][i];
        ctrlpoints[2][2][i] = (ctrlpoints[3][2][i] - ctrlpoints[3][3][i]) + (ctrlpoints[2][3][i] - ctrlpoints[3][3][i]) +  ctrlpoints[3][3][i];
    }
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &ctrlpoints[0][0][0]);
    glEnable(GL_MAP2_VERTEX_3);
}

Pos CFace::GetPosFromUV(double u,double v)const{
    //全ての曲線の差分値の合計
    Pos delta[4];
    //計算原点をセット
    delta[0] = this->GetPosFromUVSquare(  u,1.0);
    delta[1] = this->GetPosFromUVSquare(  0,  v);
    delta[2] = this->GetPosFromUVSquare(  u,  0);
    delta[3] = this->GetPosFromUVSquare(1.0,  v);
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
        //glColor4f(currentColor[0],currentColor[1],currentColor[2], 0.1);
        glDepthMask(GL_FALSE);

        //this->DefineMap2();//二次元エバリュエータ定義
        const double FACE_DIVIDE = 10.0;//面分割数
        for (int j = 0; j < FACE_DIVIDE; j++){
          for (int i = 0; i < FACE_DIVIDE; i++){
              glBegin(GL_LINE_LOOP);

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
    }else{

        //外側
        glBegin(GL_LINE_LOOP);
        for(int i=0;i<this->edges.size();i++){
            glVertex3f(this->GetPointSequence(i)->x(),this->GetPointSequence(i)->y(), this->GetPointSequence(i)->z());
        }
        glEnd();
    }

    //メッシュ描画
    QVector<CEdge*> ee = {this->GetEdgeSequence(0),this->GetEdgeSequence(1),this->GetEdgeSequence(2),this->GetEdgeSequence(3)};
    int count_max = std::min(this->GetEdgeSequence(0)->divide,this->GetEdgeSequence(2)->divide);
    for(int i=1;i<count_max;i++){
        glBegin(GL_LINES);
        Pos p1 ,p2;
        p1 = ee[0]->GetDivisionPoint(i);
        if((*ee[0]->end - *ee[0]->start).DotPos(*ee[2]->end - *ee[2]->start) < 0)p2 = ee[2]->GetDivisionPoint(count_max-i);
        else                                                                     p2 = ee[2]->GetDivisionPoint(i);
        glVertex3f(p1.x(),p1.y(),p1.z());
        glVertex3f(p2.x(),p2.y(),p2.z());
        glEnd();
    }
    count_max = std::min(this->GetEdgeSequence(1)->divide,this->GetEdgeSequence(3)->divide);
    for(int i=1;i<count_max;i++){
        glBegin(GL_LINES);
        Pos p1 ,p2;
        p1 = ee[1]->GetDivisionPoint(i);
        if((*ee[1]->end - *ee[1]->start).DotPos(*ee[3]->end - *ee[3]->start) < 0)p2 = ee[3]->GetDivisionPoint(count_max-i);
        else                                                                     p2 = ee[3]->GetDivisionPoint(i);
        glVertex3f(p1.x(),p1.y(),p1.z());
        glVertex3f(p2.x(),p2.y(),p2.z());
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

