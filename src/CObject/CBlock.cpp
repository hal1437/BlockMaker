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


bool CBlock::Creatable(QVector<CObject*> values){
    //まず点以外が4つ
    if(std::count_if(values.begin(),values.end(),[](CObject* p){return p->is<CEdge>();}) == 4){
        //構成点カウント
        std::map<Pos,int> map;
        for(CObject* p:values){
            CEdge* e = dynamic_cast<CEdge*>(p);
            map[*e->start]++;
            map[*e->end]++;
        }
        //構成点合計が2であれば
        return std::all_of(map.begin(),map.end(),[](std::pair<Pos,int> v){
            return v.second == 2;
        });
    }

    //CRect一つでも可
    /*
    if(values.size() == 1 && values[0]->is<CRect>()){
        return true;
    }*/

    return false;
}

void CBlock::SetEdgeAll(QVector<CEdge*> lines){
    //CRect一つでも可
    this->lines = lines;
}

void CBlock::SetEdge(int index,CEdge* line){
    this->lines[index] = line;
}

CEdge* CBlock::GetEdge(int index)const{
    return this->lines[index];
}

void CBlock::Draw(QPainter& painter)const{
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
    QVector<Pos> pp;
    //点を集結
    for(CEdge* line:lines){
        if(!exist(pp,*line->start))pp.push_back(*line->start);
        if(!exist(pp,*line->end  ))pp.push_back(*line->end);
    }

    //左下の探索
    QVector<Pos> hit;
    Pos corner;//左下
    std::sort(pp.begin(),pp.end(),[](Pos p1,Pos p2){return std::tie(p1.x(),p1.y()) < std::tie(p2.x(),p2.y());});//X座標が小さい順
    hit.push_back(pp[0]);//もっともX座標の小さいもの
    hit.push_back(pp[1]);//二番目にX座標の小さいもの
    std::sort(pp.begin(),pp.end(),[](Pos p1,Pos p2){return std::tie(p1.y(),p1.x()) < std::tie(p2.y(),p2.x());});//Y座標が小さい順
    hit.push_back(pp[0]);//もっともY座標の小さいもの
    hit.push_back(pp[1]);//二番目にY座標の小さいもの
    //hitに二回入った奴が左下
    for(int i=0;i<4;i++){
        Pos piv = hit[i];
        for(int j=i+1;j<4;j++){
            if(piv == hit[j]){
                corner = piv;
                i=4;//即座に終了
                break;
            }
        }
    }


    //index回だけ連鎖させる
    Pos ans = corner;
    Pos old = corner; //反復連鎖防止
    QVector<Pos> candidate;//連鎖候補
    for(int i=0;i<index%4;i++){
        //ansを含むlineを探す
        for(CEdge* line:lines){
            if(ans == *line->start && old != *line->end){
                candidate.push_back(*line->end);
            }else if(ans == *line->end && old != *line->start){
                candidate.push_back(*line->start);
            }
        }
        //選定
        old = ans;
        if(candidate.size() == 2){
            //二択
            if(Pos::Angle(candidate[0]-corner,candidate[1]-corner) > Pos::Angle(candidate[1]-corner,candidate[0]-corner)){
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
    return ans;
}


CBlock::CBlock()
{
}
CBlock::CBlock(QVector<CEdge*> lines):
    lines(lines)
{
}

CBlock::~CBlock()
{

}

