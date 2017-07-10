#include "CFace.h"

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
    Pos cross = (*points[1] - *points[0]).Cross
                (*points[2] - *points[0]);
    for(int i=3;i<point_maps.size();i++){
        double d = (*points[i] - *points[0]).DotPos(cross);
        if(!NearlyEqual(d,0)){
            return false;
        }
    }
    return true;
}

bool CFace::isParadox()const{
    if(this->edges.size() < 3)return false;
    else {
        //矛盾がないか確認する。
        for(int i=2;i<this->edges.size();i++){
            if(this->isComprehension(*this->GetPoint(i)) == false)return false;
        }
    }
    return true;
}

bool CFace::isComprehension(Pos pos)const{
    if(this->edges.size()<3)return true;
    else {
        //法線ベクトルとの内積が0であれば平面上に存在する。
        return this->GetNorm().DotPos(pos) == 0;
    }
}

Pos CFace::GetNorm()const{
    Pos vec1,vec2;
    CPoint *p1,*p2,*p3,*p4;
    p1 = this->GetPoint(0);
    p2 = this->GetPoint(1);

    vec1 = *p2 - *p1;
    vec2 = *this->GetPoint(this->edges.size()-1) - *this->GetPoint(0);

    if(this->edges.size() < 2 || (vec1-vec2).Length()<SAME_POINT_EPS)return Pos();
    else{
        return Pos(vec1.Cross(vec2)).GetNormalize();
    }
}
CPoint* CFace::GetBasePoint()const{
    QVector<CPoint*> pp;
    //点を集結
    for(CEdge* line:this->edges){
        if(!exist(pp,line->start))pp.push_back(line->start);
        if(!exist(pp,line->end  ))pp.push_back(line->end);
    }
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
CPoint* CFace::GetPoint(int index)const{
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
        qDebug() << "CFace::GetPoint(" << index << ") error. answer is nullptr.";
    }
    return ans;
}

CEdge* CFace::GetEdgeSeqence(int index){
    //index番目の点とindex+1番目の点を含む点を持つエッジを探す
    return *std::find_if(this->edges.begin(),this->edges.end(),[&](CEdge* edge){
        CPoint*  p1 = this->GetPoint(index);
        CPoint*  p2 = this->GetPoint(index+1);
        return (edge->start == p1 && edge->start == p2) ||
               (edge->start == p2 && edge->start == p1);
    });
}
QVector<CPoint*> CFace::GetAllNodes(){
    QVector<CPoint*> ans;
    for(CEdge* e:this->edges){
        for(CPoint* p:e->GetAllNodes()){
            ans.push_back(p);
        }
    }
    std::sort(ans.begin(),ans.end());
    ans.erase(std::unique(ans.begin(),ans.end()),ans.end());
    return ans;
}

bool CFace::Draw(QPainter& painter)const{
    return true;
}
bool CFace::DrawGL(Pos,Pos)const{
    if(!this->isVisible())return true;
    if(this->isPolygon()){
        //薄い色に変更
        float currentColor[4];
        glGetFloatv(GL_CURRENT_COLOR,currentColor);
        glColor4f(currentColor[0],currentColor[1],currentColor[2], 0.1);
        glDepthMask(GL_FALSE);

        //中を塗る
        glBegin(GL_POLYGON);
        for(int i=0;i<this->edges.size();i++){
            CPoint* p1 = this->GetPoint(i);
            CPoint* p2 = this->GetPoint((i+1)%this->edges.size());
            //エッジを特定
            CEdge* current = *std::find_if(this->edges.begin(),this->edges.end(),[&](CEdge* v){
                return (v->start==p1 &&v->end==p2)||(v->start==p2 &&v->end==p1);
            });
            //正方向ループ
            if(p1 == current->start){
                for(double j=0;j<=1;j+=1.0/CEdge::LINE_NEAR_DIVIDE){
                    Pos p = current->GetMiddleDivide(j);
                    glVertex3f(p.x(),p.y(), p.z());
                }
            }else{
                for(double j=1;j>=0;j-=1.0/CEdge::LINE_NEAR_DIVIDE){
                    Pos p = current->GetMiddleDivide(j);
                    glVertex3f(p.x(),p.y(), p.z());
                }
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
            glVertex3f(this->GetPoint(i)->x(),this->GetPoint(i)->y(), this->GetPoint(i)->z());
        }
    }
    glEnd();

    return true;
}
bool CFace::DrawNormArrowGL()const{
    Pos center;
    for(int i=0;i<this->edges.size();i++){
        center += *this->GetPoint(i);
    }
    center /= this->edges.size();

    glBegin(GL_LINES);
    glVertex3f(center.x(),center.y(), center.z());
    glVertex3f((center+this->GetNorm()*100).x(),(center+this->GetNorm()*100).y(), (center+this->GetNorm()*100).z());
    glEnd();
    return true;
}


bool CFace::Move(const Pos& diff){
    QVector<CPoint*> pp;
    for(int i=0;i<this->edges.size();i++){
        for(int j=0;j<this->edges[i]->GetPosSequenceCount();j++){
            pp.push_back(this->edges[i]->GetPosSequence(j));
        }
    }
    //排他
    std::sort(pp.begin(),pp.end());
    pp.erase(std::unique(pp.begin(),pp.end()),pp.end());

    //移動
    for(CPoint* p:pp){
        p->Move(diff);
    }
    return true;
}

//近接点
Pos CFace::GetNearPos (const Pos&)const{
    return Pos();
}
Pos CFace::GetNearLine(const Pos& pos1,const Pos& pos2)const{
    return Pos();
}

CFace::CFace(QObject* parent):
    CObject(parent)
{
}

CFace::~CFace()
{
}

