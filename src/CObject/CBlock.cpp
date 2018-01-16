#include "CBlock.h"

bool CBlock::Creatable(QList<CObject*> values){
    if(values.size()==1 && values[0]->is<CBlock>())return true;

    //全てがCFaceである
    if(std::any_of(values.begin(),values.end(),[](CObject* p){return !p->is<CFace>();}))return false;

    //6面存在する
    if(values.size() != 6)return false;

    //閉じた立体テスト
    std::map<CPoint*,int> point_maps;
    for(CObject* v: values){
        for(CEdge* e : dynamic_cast<CFace*>(v)->edges){
            point_maps[e->start]++;
            point_maps[e->end]++;
        }
    }
    if(!std::all_of(point_maps.begin(),point_maps.end(),[](std::pair<CObject*,int> c){return c.second==6;}))return false;

    return true;
}
void CBlock::Create(QList<CFace*> faces){
    this->faces = faces;
    for(CFace* f :this->faces){
        f->SetFaceBlend(false); //面を非透過に
        this->ObserveChild(f);  //監視
    }
}

CPoint* CBlock::GetBasePoint()const{
    double LIMIT_LENGTH = 0;
    QList<CPoint*> vertex = this->GetAllPoints();
    for(CPoint* pos:vertex){
        QList<double> vs = {std::abs(pos->x()),std::abs(pos->y()),std::abs(pos->z()),LIMIT_LENGTH};
        LIMIT_LENGTH = *std::max_element(vs.begin(),vs.end());
    }
    //角の算出
    Pos limit(-LIMIT_LENGTH,-LIMIT_LENGTH,-LIMIT_LENGTH);
    return *std::min_element(vertex.begin(),vertex.end(),[&](CPoint* lhs,CPoint* rhs){
        return ((*lhs-limit).Length() < (*rhs-limit).Length());
    });
}
CEdge*  CBlock::GetBaseEdge ()const{
    CPoint* base = this->GetBasePoint();

    //基準点を含むエッジ
    QList<CEdge*> ee;
    for(CEdge* e :this->GetAllEdges()){
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
QList<CPoint*> CBlock::GetAllPoints()const{
    QList<CPoint*>pp;
    for(CFace* face:faces){
        for(CEdge* edge:face->edges){
            pp.push_back(edge->start);
            pp.push_back(edge->end);
        }
    }
    //重複を削除
    std::sort(pp.begin(),pp.end());
    pp.erase(std::unique(pp.begin(),pp.end()),pp.end());
    return pp;
}
QList<CEdge*> CBlock::GetAllEdges()const{
    QList<CEdge*>ee;
    for(CFace* face:faces){
        for(CEdge* edge:face->edges){
            ee.push_back(edge);
        }
    }
    //重複を削除
    std::sort(ee.begin(),ee.end());
    ee.erase(std::unique(ee.begin(),ee.end()),ee.end());
    return ee;
}
QList<CFace*> CBlock::GetAllFaces()const{
    return this->faces;
}

double CBlock::GetLength_impl(Quat convert){
    QList<CPoint*> pp;
    pp = this->GetAllPoints();
    std::for_each(pp.begin(),pp.end(),[&](CPoint* pos){*pos = pos->Dot(convert);});
    double begin = (*std::min_element(pp.begin(),pp.end(),[](CPoint* lhs,CPoint* rhs){return lhs->mat[0] < rhs->mat[0];}))->mat[0];
    double end   = (*std::max_element(pp.begin(),pp.end(),[](CPoint* lhs,CPoint* rhs){return lhs->mat[0] < rhs->mat[0];}))->mat[0];
    return end - begin;
}
double CBlock::GetLengthX(){
    return this->GetLength_impl(Quat::getIdentityMatrix());
}
double CBlock::GetLengthY(){
    return this->GetLength_impl(Quat({0,0,0,0,
                                      1,0,0,0,
                                      0,0,0,0,
                                      0,0,0,0}));
}
double CBlock::GetLengthZ(){
    return this->GetLength_impl(Quat({0,0,0,0,
                                      0,0,0,0,
                                      1,0,0,0,
                                      0,0,0,0}));
}
void CBlock::DrawGL(Pos,Pos)const{
    if(!this->isVisible())return;
    return;
}

//近接点
Pos CBlock::GetNearPos (const Pos& )const{
    return Pos();
}
Pos CBlock::GetNearLine(const Pos& ,const Pos& )const{
    return Pos();
}

CPoint* CBlock::GetPointSequence(int index)const{
    //極大値
    CPoint* ans[8];
    ans[0] = this->GetBasePoint();
    if(index == 0)return ans[0];

    //方向エッジ算出
    CEdge* base_edges[3]={nullptr,nullptr,nullptr};
    QList<CEdge*> edges = this->GetAllEdges();

    //edgesをクローンに入れ替え
    QList<CEdge*> swap_tmp;
    for(CEdge* edge:edges){
        CEdge* clone = dynamic_cast<CEdge*>(edge->Clone());
        clone->start = edge->start;
        clone->end   = edge->end;
        swap_tmp.push_back(clone);
    }
    edges = swap_tmp;

    //原点を含むエッジ以外を削除
    edges.erase(std::remove_if(edges.begin(),edges.end(),[&](CEdge* edge){
        return (edge->start != ans[0] && edge->end != ans[0]);
    }),edges.end());

    //始点と終点を整理
    for(CEdge* edge:edges){
        if(edge->end == ans[0])std::swap(edge->start,edge->end);
    }

    //base0との内角が最も大きいものを0番目に
    int max_index = std::distance(edges.begin(),std::max_element(edges.begin(),edges.end(),[&](CEdge* lhs,CEdge* rhs){
        return ((*lhs->end - *lhs->start).GetNormalize().DotPos(Pos(1,0,0)) <
                (*rhs->end - *rhs->start).GetNormalize().DotPos(Pos(1,0,0)));
    }));
    base_edges[0] = edges[max_index];

    //1と2のエッジも代入
    base_edges[1] = edges[(max_index+1)%3];
    base_edges[2] = edges[(max_index+2)%3];


    //外角の方向が正しくなければ入れ替え
    if(Pos((*base_edges[0]->end - *base_edges[0]->start).Cross(*base_edges[1]->end - *base_edges[1]->start)).DotPos
           (*base_edges[2]->end - *base_edges[2]->start) < 0)std::swap(base_edges[1],base_edges[2]);

    //方向エッジ上の点
    ans[1] =((base_edges[0]->start != ans[0]) ? base_edges[0]->start : base_edges[0]->end);
    ans[3] =((base_edges[1]->start != ans[0]) ? base_edges[1]->start : base_edges[1]->end);
    ans[4] =((base_edges[2]->start != ans[0]) ? base_edges[2]->start : base_edges[2]->end);
    if(index == 1 || index == 3 || index == 4)return ans[index];

    //上の三点に近接する点
    int in_index[] = {1,3,4};
    int out_index[] = {2,7,5};

    //connect2つと隣接している、かつans[0]でない点
    for(int i=0;i<3;i++){
        CPoint* connect[2] = {ans[in_index[i]],ans[in_index[(i+1)%3]]};

        //カウント開始
        std::map<CPoint*,int>maps;
        for(CEdge* edge: this->GetAllEdges()){
            if(edge->start == connect[0] || edge->start == connect[1] ||
               edge->end   == connect[0] || edge->end   == connect[1]){
                maps[edge->start]++;
                maps[edge->end]++;
            }
        }
        for(std::pair<CPoint*,int> p:maps){
            if(p.second == 2 && p.first != ans[0]){
                ans[out_index[i]] = p.first;
            }
        }
    }
    if(index == 2 || index == 7 || index == 5){
        return ans[index];
    }

    //ここに到達できるのは6のみ
    //それ以外の点
    QList<CPoint*> pos = this->GetAllPoints();
    for(int i=0;i<8;i++){
        if(i!=6)pos.removeAll(ans[i]);
    }
    ans[6] = pos.first();

    return ans[6];
}
CEdge* CBlock::GetEdgeSequence(int index) const{
    //始点と終点を持つ点を探す
    QList<CEdge*> edges = this->GetAllEdges();
    CPoint* p1 = this->GetPointSequence(edge_comb[index][0]);
    CPoint* p2 = this->GetPointSequence(edge_comb[index][1]);
    CEdge* edge = *std::find_if(edges.begin(),edges.end(),[&](CEdge* e){
        return (e->start == p1 && e->end == p2) || (e->start == p2 && e->end == p1);
    });
    return edge;
}

bool CBlock::isEdgeReverse(int index){
    //始点と終点を持つ点を探す
    QList<CEdge*> edges = this->GetAllEdges();
    CPoint* p1 = this->GetPointSequence(edge_comb[index][0]);
    CPoint* p2 = this->GetPointSequence(edge_comb[index][1]);
    CEdge* edge = *std::find_if(edges.begin(),edges.end(),[&](CEdge* e){
        return (e->start == p1 && e->end == p2) || (e->start == p2 && e->end == p1);
    });
    if(edge->start == p2)return true;
    else return false;
}


//子の操作
CFace*   CBlock::GetFace (int index){
    return this->faces[index];
}
CObject* CBlock::GetChild(int index){
    return this->GetFace(index);
}
void CBlock::SetChild(int index,CObject* obj){
    this->faces[index] = dynamic_cast<CFace*>(obj);
}

int CBlock::GetChildCount()const{
    return this->faces.size();
}

CObject* CBlock::Clone()const{
    CBlock* new_obj = new CBlock();
    new_obj->faces = this->faces;
    return new_obj;
}

CBlock::CBlock(QObject* parent):
    CObject(parent)
{
}

CBlock::~CBlock()
{
}
