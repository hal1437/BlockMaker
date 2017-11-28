#include "CFace.h"

CFace* CFace::base[3];


#define SWITCHING_STRING_TO_boundary_TYPE(TYPE,COMP)\
if(COMP == #TYPE)return Boundary::Type::TYPE;

#define SWITCHING_boundary_TYPE_TO_STRING(TYPE,COMP)\
if(COMP == Boundary::Type::TYPE) return #TYPE;\

Boundary::Type Boundary::StringToBoundaryType(QString str){
    SWITCHING_STRING_TO_boundary_TYPE(empty        ,str)
    SWITCHING_STRING_TO_boundary_TYPE(patch        ,str)
    SWITCHING_STRING_TO_boundary_TYPE(wall         ,str)
    SWITCHING_STRING_TO_boundary_TYPE(symmetryPlane,str)
    SWITCHING_STRING_TO_boundary_TYPE(cyclic       ,str)
    SWITCHING_STRING_TO_boundary_TYPE(cyclicAMI    ,str)
    SWITCHING_STRING_TO_boundary_TYPE(wedge        ,str)
    SWITCHING_STRING_TO_boundary_TYPE(undefined    ,str)
    return Boundary::Type::undefined;
}
QString Boundary::BoundaryTypeToString(Boundary::Type type){
    SWITCHING_boundary_TYPE_TO_STRING(empty        ,type)
    SWITCHING_boundary_TYPE_TO_STRING(patch        ,type)
    SWITCHING_boundary_TYPE_TO_STRING(wall         ,type)
    SWITCHING_boundary_TYPE_TO_STRING(symmetryPlane,type)
    SWITCHING_boundary_TYPE_TO_STRING(cyclic       ,type)
    SWITCHING_boundary_TYPE_TO_STRING(cyclicAMI    ,type)
    SWITCHING_boundary_TYPE_TO_STRING(wedge        ,type)
    SWITCHING_boundary_TYPE_TO_STRING(undefined    ,type)
    return "none";
}

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
void CFace::Create(QVector<CEdge*> edges){
    this->edges = edges;
    for(int i=0;i<edges.size();i++){
        if(std::count(edges.begin(),edges.end(),edges[i]) > 1){
            qDebug() <<"warning! " << this << " is having same edges";
        }
    }
    if(edges.size()==4){
        this->ReorderEdges();
        this->RecalcMesh();
    }
    for(CEdge* e :this->edges){
        this->ObserveChild(e);
    }
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
    delta[0] += (this->GetEdgeMiddle(0,u) - delta[0]) * (1.0-v);
    delta[1] += (this->GetEdgeMiddle(1,v) - delta[1]) * u;
    delta[2] += (this->GetEdgeMiddle(2,1.0-u) - delta[2]) * v;
    delta[3] += (this->GetEdgeMiddle(3,1.0-v) - delta[3]) * (1.0-u);
    //GetPosFromUVSquareとの差分を取得
    for(int i=0;i<4;i++){
        delta[i] -= this->GetPosFromUVSquare(u,v);
    }

    return this->GetPosFromUVSquare(u,v) + (delta[0]+delta[1]+delta[2]+delta[3]);
}
Pos CFace::GetPosFromUVSquare(double u,double v)const{
    //双1次曲面の式
    //S(u,v)=(1-u)(1-v)P_{0}+u(1-v)P_{1}+(1-u)vP_{2}+uvP_{3};
    Pos p[4] = {this->GetEdgeMiddle(0,0),
                this->GetEdgeMiddle(1,0),
                this->GetEdgeMiddle(2,0),
                this->GetEdgeMiddle(3,0)};
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
            }
            if(ans == line->end && old != line->start){
                candidate.push_back(line->start);
            }
        }
        //選定
        old = ans;
        if(candidate.size() == 2){
            //X基底に近いエッジを選択
            double d1 = (*candidate[0]-*ans).GetNormalize().DotPos(Pos(1,0,0));
            double d2 = (*candidate[1]-*ans).GetNormalize().DotPos(Pos(1,0,0));
            //X方向評価可能
            if(std::abs(d1-d2) > SAME_POINT_EPS){
                //X方向評価
                if(d1 > d2){
                    ans = candidate[0];
                }else{
                    ans = candidate[1];
                }
            }else{
                //Y方向評価
                if((*candidate[0]-*ans).GetNormalize().DotPos(Pos(0,1,0)) > (*candidate[1]-*ans).GetNormalize().DotPos(Pos(0,1,0))){
                    ans = candidate[0];
                }else{
                    ans = candidate[1];
                }
            }
        }else if(candidate.size() == 1){
            ans = candidate[0];
        }else{
            ans = corner;
            qDebug() << "?";
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
    return *it;
}
void CFace::DrawGL(Pos,Pos)const{
    if(!this->isVisible())return;

    //色を保存
    float currentColor[4];
    glGetFloatv(GL_CURRENT_COLOR,currentColor);

    //完全透過色であれば色を法線にあった色に変更
    if(currentColor[3] == 0){
        Pos norm = this->GetNorm();
        glColor4f(std::abs(norm.x()),
                  std::abs(norm.y()),
                  std::abs(norm.z()),
                  1.0);
    }

    if(!exist(CFace::base,this)){
        //薄い色に変更
        if(this->isFaceBlend()){
            //Aを薄める
            glColor4f(currentColor[0],currentColor[1],currentColor[2], 0.1);
        }else{
            //ブロック構築用面
            glColor4f(currentColor[0] * 0.2 + 0.5,
                      currentColor[1] * 0.2 + 0.5,
                      currentColor[2] * 0.2 + 0.6,
                      1.0);
        }

        if(this->edges.size() != 4){
            //多角形描画
            glBegin(GL_POLYGON);
            for(CPoint* pp :this->GetAllChildren()){
                glVertex3f(pp->x(),pp->y(),pp->z());
            }
            glEnd();
        }else{
            //通常面描画
            DrawFillGL();
        }
    }else{

        //枠のみ描画
        glBegin(GL_LINE_LOOP);
        for(int i=0;i<this->edges.size();i++){
            glVertex3f(this->GetPointSequence(i)->x(),
                       this->GetPointSequence(i)->y(),
                       this->GetPointSequence(i)->z());
        }
        glEnd();
    }
    //色を復元
    glColor4f(currentColor[0],currentColor[1],currentColor[2], currentColor[3]);

    glDepthMask(GL_TRUE);

}
bool CFace::DrawNormArrowGL()const{
    /* 適切な値を示さない可能性アリ */
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
void CFace::DrawFillGL()const{
    //メッシュ分割に依存した描画
    glBegin(GL_QUADS);
    for(double i=0;i<this->mesh_memory.size()-1;i++){//u方向ループ
        for(double j=0;j<this->mesh_memory.first().size()-1;j++){//v方向ループ
            Pos pp[4] = {
                this->mesh_memory[ i ][ j ],
                this->mesh_memory[i+1][ j ],
                this->mesh_memory[i+1][j+1],
                this->mesh_memory[ i ][j+1],
            };
            for(int k=0;k<4;k++){
                glVertex3f(pp[k].x(),pp[k].y(),pp[k].z());
            }
        }
    }
    glEnd();

}
void CFace::DrawMeshGL()const{
    //u方向
    for(double i=0;i<this->mesh_memory.size()-1;i++){//u方向ループ
        for(double j=1;j<this->mesh_memory.first().size()-1;j++){//v方向ループ
            //描画
            glBegin(GL_LINE_STRIP);
            glVertex3f(this->mesh_memory[i][j].x(),this->mesh_memory[i][j].y(),this->mesh_memory[i][j].z());
            glVertex3f(this->mesh_memory[i+1][j].x(),this->mesh_memory[i+1][j].y(),this->mesh_memory[i+1][j].z());
            glEnd();

        }
    }
    //v方向
    for(double i=1;i<this->mesh_memory.size()-1;i++){//u方向ループ
        for(double j=0;j<this->mesh_memory.first().size()-1;j++){//v方向ループ
            //描画
            glBegin(GL_LINE_STRIP);
            glVertex3f(this->mesh_memory[i][j].x(),this->mesh_memory[i][j].y(),this->mesh_memory[i][j].z());
            glVertex3f(this->mesh_memory[i][j+1].x(),this->mesh_memory[i][j+1].y(),this->mesh_memory[i][j+1].z());
            glEnd();

        }
    }
}

void CFace::ReorderEdges(){
    QVector<CEdge*> ans;
    //整合確認
    for(int i=0;i<this->edges.size();i++){
        ans.push_back(this->GetEdgeSequence(i));
    }
    this->edges = ans;

    //反転確認
    this->reorder.resize(this->edges.size());
    for(int i=0;i<this->edges.size();i++){
        CPoint* pp = this->GetPointSequence(i);
        if     (pp == this->edges[i]->start)this->reorder[i] =  1;
        else if(pp == this->edges[i]->end  )this->reorder[i] = -1;
        else{
            qDebug() << "?";
            this->reorder[i] =  0;
        }
    }
}
void CFace::RecalcMesh(){
    //メッシュ分割描画
    int u_max = std::min(this->GetEdge(0)->getDivide(),this->GetEdge(2)->getDivide())+1;//u方向分割
    int v_max = std::min(this->GetEdge(1)->getDivide(),this->GetEdge(3)->getDivide())+1;//v方向分割
    if(u_max < 2)u_max = 2;
    if(v_max < 2)v_max = 2;

    this->mesh_memory.resize(u_max);
    for(double i=0;i<u_max;i++){//u方向ループ
        QVector<Pos> v_pp;//v方向点群
        v_pp.resize(v_max);
        for(double j=0;j<v_max;j++){//v方向ループ
            //倍率計算
            double rate_0 = CEdge::GetDivisionRate(u_max-1,this->GetGrading(0),i);
            double rate_2 = CEdge::GetDivisionRate(u_max-1,this->GetGrading(2).GetReverse(),i);
            double rate_1 = CEdge::GetDivisionRate(v_max-1,this->GetGrading(1),j);
            double rate_3 = CEdge::GetDivisionRate(v_max-1,this->GetGrading(3).GetReverse(),j);

            double rate_p1 = (rate_2-rate_0) * (  j  /(v_max-1)) + rate_0;
            double rate_p2 = (rate_1-rate_3) * (  i  /(u_max-1)) + rate_3;

            //座標計算
            v_pp[j] = this->GetPosFromUV(rate_p1,rate_p2);
        }
        this->mesh_memory[i] = v_pp;
    }
}

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
    if(this->reorder.isEmpty())return (this->GetEdge(index)->GetMiddleDivide(t));
    if(this->reorder[index] == -1)return this->GetEdge(index)->GetMiddleDivide(1.0 - t);
    else                          return this->GetEdge(index)->GetMiddleDivide(t);
}
CEdge::Grading   CFace::GetGrading(int index)const{
    if(this->reorder.isEmpty())return (this->GetEdge(index)->getGrading());
    if(this->reorder[index] == -1)return (this->GetEdge(index)->getGrading().GetReverse());
    else                          return (this->GetEdge(index)->getGrading());
}

Pos CFace::GetNearPos (const Pos& pos)const{
    return Collision::GetHitPosFaceToPoint(this->GetNorm(),*this->GetAllChildren().first(),pos);
}
Pos CFace::GetNearLine(const Pos& ,const Pos& )const{
    return Pos();
}

CObject* CFace::Clone()const{
    CFace* new_obj = new CFace();
    QVector<CEdge*> edges;
    for(CEdge* edge:this->edges){
        edges.push_back(dynamic_cast<CEdge*>(edge->Clone()));
    }
    //構成点マージ
    for(CEdge* e1: edges){
        for(CEdge* e2: edges){
            if(*e1->start == *e2->start)e2->start = e1->start;
            if(*e1->start == *e2->end  )e2->end   = e1->start;
            if(*e1->end   == *e2->start)e2->start = e1->end;
            if(*e1->end   == *e2->end  )e2->end   = e1->end;
        }
    }
    new_obj->Create(edges);
    new_obj->name = this->name;
    new_obj->boundary = this->boundary;
    return new_obj;
}

CFace::CFace(QObject* parent):
    CObject(parent)
{
    this->name = "Noname";
    this->boundary.type = Boundary::Type::undefined;
    this->boundary.name = "境界条件未定義";
}

CFace::~CFace(){}

void CFace::ChangeChildCallback(QVector<CObject*> edges){
    for(CEdge* edge:this->edges){
        if(exist(edges,edge)){
            //対角エッジの分割数同期
            for(int i=0;i<this->edges.size();i++){
                if(this->edges[i] == edge){
                    this->edges[(i+2)%this->edges.size()]->setDivide(edge->getDivide());
                }
            }
        }
    }
    //
    //メッシュ再計算
    this->RecalcMesh();
}

double CFace::GetLengthFaceToLine(Pos center ,Pos dir){
    //法線ベクトルの算出
    Pos norm = this->GetNorm();

    //面と平行
    if((dir).DotPos(norm) == 0)return false;

    //交点を取得
    Pos p = Collision::GetHitPosFaceToLine(norm,*this->GetPointSequence(0),center,dir);
    return dir.GetNormalize().DotPos(p - center);
}

bool CFace::CheckHitFaceToLine(Pos center ,Pos dir){
    //法線ベクトルの算出
    Pos norm = this->GetNorm();

    //面と平行
    if((dir).DotPos(norm) == 0)return false;

    //交点を取得
    Pos p = Collision::GetHitPosFaceToLine(norm,*this->GetPointSequence(0),center,dir);

    //四角形内であるか
    double sum=0;
    for(int i=0;i<this->edges.size();i++){
        sum += Pos::Angle(*this->GetPointSequence(i)-p,*this->GetPointSequence((i+1)%this->edges.size())-p);
    }
    if(std::abs(sum-360) > 0.000001){
        return false;
    }else{
        return true;
    }
}


