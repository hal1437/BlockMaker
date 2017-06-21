#include "CBlock.h"

//分割点取得
Pos CBlock::GetDivisionPoint(int edge_index,int count_index)const{
    double A,B,sum=0,p,d,L;
    CPoint *start,*end;

    //各パラメータ取得
    if(edge_index == 0 || edge_index == 2 || edge_index == 4  || edge_index == 6 ){ //X
        p = this->grading_args[0];
        d = this->div[0];
    }
    if(edge_index == 1 || edge_index == 3 || edge_index == 5  || edge_index == 7 ){ //Y
        p = this->grading_args[1];
        d = this->div[1];
    }
    if(edge_index == 8 || edge_index == 9 || edge_index == 10 || edge_index == 11){ //Z
        p = this->grading_args[2];
        d = this->div[2];
    }
    //EdgeGradingならばpを上書き
    if(grading == GradingType::EdgeGrading){
        p = this->grading_args[edge_index];
    }
    //始点と終点を取得
    QVector<QVector<int>> edge_comb = {{0,1}, //0
                                       {1,2}, //1
                                       {3,2}, //2
                                       {0,3}, //3
                                       {4,5}, //4
                                       {5,6}, //5
                                       {7,6}, //6
                                       {4,7}, //7
                                       {0,4}, //8
                                       {1,5}, //9
                                       {2,6}, //10
                                       {3,7}};//11
    start = this->GetClockworksPos(edge_comb[edge_index][0]);
    end   = this->GetClockworksPos(edge_comb[edge_index][1]);
    L = (*end-*start).Length();

    //指数関数パラメータ計算
    B = log(p) / (d-1);
    for(int i=1;i<=d;i++)sum += exp(B*i);
    A = L / sum;

    //指定番号までの総和
    double sum_rate=0;
    for(int i=1;i<=count_index;i++){
        sum_rate += A*exp(B*i);
    }
    Pos ans = (*end-*start).GetNormalize() * sum_rate;
    return ans + *start;
}

double CBlock::GetLength_impl(Quat convert){
    QVector<CPoint*> pp;
    pp = this->GetVerticesPos();
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

bool CBlock::DrawGL(Pos,Pos)const{
    //薄い色に変更
    float oldColor[4];
    glGetFloatv(GL_CURRENT_COLOR,oldColor);
    glColor4f(0.5,
              0.5,
              0.5,
              1);

    for(CFace* face:this->faces){
        //中を塗る
        glBegin(GL_TRIANGLE_FAN);
        for(int i=0;i<face->edges.size();i++){
            glVertex3f(face->GetPoint(i)->x(),face->GetPoint(i)->y(), face->GetPoint(i)->z());
        }
        glEnd();
    }

    //分割線を描画
    glColor4f(0.1,
              0.1,
              0.1,
              1);
    if(this->grading == GradingType::SimpleGrading){
        int edge_index[3][4] = {{0,2,6,4},{1,3,7,5},{8,9,10,11}};
        for(int i=0;i<3;i++){
            for(int j=0;j<this->div[i];j++){
                glBegin(GL_LINE_LOOP);
                for(int k=0;k<4;k++){
                    //線の分割描画
                    Pos p = this->div_pos[edge_index[i][k]][j];
                    glVertex3f(p.x(),p.y(),p.z());
                }
                glEnd();
            }
        }
    }
    //色を復元
    glColor4f(oldColor[0],oldColor[1],oldColor[2], oldColor[3]);

}
bool CBlock::Move  (const Pos& diff){
}

//近接点
Pos CBlock::GetNearPos (const Pos& hand)const{
    return Pos();
}
Pos CBlock::GetNearLine(const Pos& pos1,const Pos& pos2)const{
    return Pos();
}


CEdge* CBlock::GetEdge(int index)const{
    QVector<CEdge*> vv;
    for(CFace* face : this->faces){
        for(CEdge* edge : face->edges){
            if(!exist(vv,edge))vv.push_back(edge);
        }
    }
    return vv[index];
}


bool CBlock::Creatable(QVector<CObject*> values){
    if(values.size()==1 && values[0]->is<CBlock>())return true;

    if(std::any_of(values.begin(),values.end(),[](CObject* p){return !p->is<CFace>();}))return false;
    if(values.size() < 6)return false;


    //閉じた立体テスト
    std::map<CPoint*,int> point_maps;
    for(CObject* v: values){
        for(CEdge* e : dynamic_cast<CFace*>(v)->edges){
            point_maps[e->start]++;
            point_maps[e->end]++;
        }
    }
    //全て6
    if(!std::all_of(point_maps.begin(),point_maps.end(),[](std::pair<CObject*,int> c){return c.second==6;}))return false;

    return true;
}

bool CBlock::Draw(QPainter& painter)const{
    //描画範囲算出
    double top,bottom,left,right;
    QVector<Pos> pp;
    for(int i=0;i<4;i++){
        pp.push_back(*this->GetClockworksPos(i));
    }
    top    = std::min_element(pp.begin(),pp.end(),[](const Pos& lhs,const Pos& rhs){return lhs.y() < rhs.y();})->y();
    bottom = std::max_element(pp.begin(),pp.end(),[](const Pos& lhs,const Pos& rhs){return lhs.y() < rhs.y();})->y();
    left   = std::min_element(pp.begin(),pp.end(),[](const Pos& lhs,const Pos& rhs){return lhs.x() < rhs.x();})->x();
    right  = std::max_element(pp.begin(),pp.end(),[](const Pos& lhs,const Pos& rhs){return lhs.x() < rhs.x();})->x();

    //多角形の描画
    QPointF vertex[4];
    for(int i=0;i<4;i++){
        vertex[i] = QPointF(pp[i].x(),pp[i].y());
    }
    painter.drawPolygon(vertex,4);

    //分割線の描画
    for(int i =0;i<=this->div[0];i++){
        painter.drawLine(QPointF(this->GetDivisionPoint(0,i).x(),this->GetDivisionPoint(0,i).y()),
                         QPointF(this->GetDivisionPoint(2,i).x(),this->GetDivisionPoint(2,i).y()));
    }
    for(int i =0;i<=this->div[1];i++){
        painter.drawLine(QPointF(this->GetDivisionPoint(1,i).x(),this->GetDivisionPoint(1,i).y()),
                         QPointF(this->GetDivisionPoint(3,i).x(),this->GetDivisionPoint(3,i).y()));
    }
}


QVector<CPoint*> CBlock::GetVerticesPos()const{
    QVector<CPoint*>pp;
    for(CFace* face:faces){
        for(int i=0;i<4;i++){
            pp.push_back(face->GetPoint(i));
        }
    }
    //重複を削除
    std::sort(pp.begin(),pp.end());
    pp.erase(std::unique(pp.begin(),pp.end()),pp.end());
    return pp;
}
CPoint* CBlock::GetClockworksPos(int index)const{
    //極大値
    QVector<CPoint*> vertex = this->GetVerticesPos();
    double LIMIT_LENGTH = 0;
    for(CPoint* pos:vertex){
        QVector<double> vs = {pos->x(),pos->y(),pos->z(),LIMIT_LENGTH};
        LIMIT_LENGTH = *std::max_element(vs.begin(),vs.end());
    }

    CORNER_LIM x_lim,y_lim,z_lim;
    if(index == 0 || index == 3 || index == 4 || index == 7)x_lim = CORNER_LIM::MIN;
    else                                                    x_lim = CORNER_LIM::MAX;
    if(index == 0 || index == 1 || index == 4 || index == 5)y_lim = CORNER_LIM::MIN;
    else                                                    y_lim = CORNER_LIM::MAX;
    if(index == 0 || index == 1 || index == 2 || index == 3)z_lim = CORNER_LIM::MIN;
    else                                                    z_lim = CORNER_LIM::MAX;

    //極値
    Pos limit = Pos(((x_lim == CORNER_LIM::MAX) ? 1 : -1),
                    ((y_lim == CORNER_LIM::MAX) ? 1 : -1),
                    ((z_lim == CORNER_LIM::MAX) ? 1 : -1)) * LIMIT_LENGTH;

    CPoint* ans = *std::min_element(vertex.begin(),vertex.end(),[&](CPoint* lhs,CPoint* rhs){
        return ((*lhs-limit).Length() < (*rhs-limit).Length());
    });
    return ans;
}


CBlock::CBlock(QObject* parent):
    CObject(parent)
{
}

CBlock::~CBlock()
{

}
void CBlock::RefreshDividePoint(){

    //更新
    this->div_pos.clear();
    this->div_pos.resize(12);
    if(this->grading == GradingType::SimpleGrading){
        for(int i=0;i<12;i++){
            QVector<Pos> pos;
            int div_index[] = {0,1,0,1,0,1,0,1,2,2,2,2};
            for(int j=0;j<this->div[div_index[i]];j++){
                //線の分割描画
                 pos.push_back(this->GetDivisionPoint(i,j));
            }
            this->div_pos[i] = pos;
        }
    }
}

