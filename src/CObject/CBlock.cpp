#include "CBlock.h"

//分割点取得
Pos CBlock::GetDivisionPoint(int edge_index,int count_index)const{
    double A,B,sum=0,p,d,L;
    Pos start,end;

    //各パラメータ取得
    if(edge_index == 0 || edge_index == 2 || edge_index == 4  || edge_index == 6 ){ //X
        p = this->grading_args[0];
        d = this->div[0];
    }
    if(edge_index == 1 || edge_index == 3 || edge_index == 5  || edge_index == 7 ){ //Y
        p = this->grading_args[1];
        d = this->div[1];
    }
    //EdgeGradingならばpを上書き
    if(grading == GradingType::EdgeGrading){
        p = this->grading_args[edge_index];
    }
    //始点と終点を取得
    if((edge_index % 4) == 0)start = this->GetClockworksPos(0),end = this->GetClockworksPos(1);
    if((edge_index % 4) == 1)start = this->GetClockworksPos(1),end = this->GetClockworksPos(2);
    if((edge_index % 4) == 2)start = this->GetClockworksPos(3),end = this->GetClockworksPos(2);
    if((edge_index % 4) == 3)start = this->GetClockworksPos(0),end = this->GetClockworksPos(3);
    L = (end-start).Length();

    //指数関数パラメータ計算
    B = log(p) / (d-1);
    for(int i=1;i<=d;i++)sum += exp(B*i);
    A = L / sum;

    //指定番号までの総和
    double sum_rate=0;
    for(int i=1;i<=count_index;i++){
        sum_rate += A*exp(B*i);
    }
    Pos ans = (end-start).GetNormalize() * sum_rate;
    return ans + start;
}

double CBlock::GetWidth(){
    QVector<Pos> pp;
    pp = this->GetVerticesPos();
    double left   = std::min_element(pp.begin(),pp.end(),[](const Pos& lhs,const Pos& rhs){return lhs.x() < rhs.x();})->x();
    double right  = std::max_element(pp.begin(),pp.end(),[](const Pos& lhs,const Pos& rhs){return lhs.x() < rhs.x();})->x();
    return right-left;
}

double CBlock::GetHeight(){
    QVector<Pos> pp;
    pp = this->GetVerticesPos();
    double top    = std::min_element(pp.begin(),pp.end(),[](const Pos& lhs,const Pos& rhs){return lhs.y() < rhs.y();})->y();
    double bottom = std::max_element(pp.begin(),pp.end(),[](const Pos& lhs,const Pos& rhs){return lhs.y() < rhs.y();})->y();
    return top-bottom;
}

bool CBlock::DrawGL(Pos,Pos)const{
    //薄い色に変更
    float currentColor[4];
    glGetFloatv(GL_CURRENT_COLOR,currentColor);
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
    //色を復元
    glColor4f(currentColor[0],currentColor[1],currentColor[2], currentColor[3]);

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
        pp.push_back(this->GetClockworksPos(i));
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


QVector<Pos> CBlock::GetVerticesPos()const{
    //0~4で巡回するように
    QVector<Pos> pp;
    for(int i=0;i<4;i++){
        pp.push_back(this->GetClockworksPos(i));
    }
    return pp;
}
Pos CBlock::GetClockworksPos(int index)const{

    return Pos();
}


CBlock::CBlock(QObject* parent):
    CObject(parent)
{
}

CBlock::~CBlock()
{

}

