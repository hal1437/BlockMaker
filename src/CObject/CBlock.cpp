#include "CBlock.h"

bool CBlock::Creatable(QVector<CObject*> values){
    //まず点以外が4つ
    if(std::count_if(values.begin(),values.end(),[](CObject* p){return !p->is<CPoint>();}) == 4){
        //構成点カウント
        std::map<Pos,int> map;
        for(CObject* p:values){
            map[p->GetJointPos(0)]++;
            map[p->GetJointPos(p->GetJointNum()-1)]++;
        }
        //構成点合計が2であれば
        return std::all_of(map.begin(),map.end(),[](std::pair<Pos,int> v){
            return v.second == 2;
        });
    }

    //CRect一つでも可
    if(values.size() == 1 && values[0]->is<CRect>()){
        return true;
    }

    return false;
}

void CBlock::SetNodeAll(QVector<CObject*> lines){
    //CRect一つでも可
    if(lines.size() == 1 && lines[0]->is<CRect>()){
        for(int i=0;i<4;i++){
            this->lines.push_back(dynamic_cast<CRect*>(lines[0])->GetLines(i));
        }
    }else{
        this->lines = lines;
    }
}

void CBlock::SetNode(int index,CObject* line){
    this->lines[index] = line;
}

CObject* CBlock::GetNode(int index)const{
    return this->lines[index];
}

void CBlock::Draw(QPainter& painter)const{
    //描画範囲算出
    double top,bottom,left,right;
    QVector<Pos> pp;
    for(int i=0;i<4;i++){
        pp.push_back(this->GetClockworksPos(i));
    }
    top    = std::min_element(pp.begin(),pp.end(),[](const Pos& lhs,const Pos& rhs){return lhs.y < rhs.y;})->y;
    bottom = std::max_element(pp.begin(),pp.end(),[](const Pos& lhs,const Pos& rhs){return lhs.y < rhs.y;})->y;
    left   = std::min_element(pp.begin(),pp.end(),[](const Pos& lhs,const Pos& rhs){return lhs.x < rhs.x;})->x;
    right  = std::max_element(pp.begin(),pp.end(),[](const Pos& lhs,const Pos& rhs){return lhs.x < rhs.x;})->x;

    //多角形の描画
    QPointF vertex[4];
    for(int i=0;i<4;i++){
        vertex[i] = QPointF(pp[i].x,pp[i].y);
    }
    painter.drawPolygon(vertex,4);

    //分割線の描画

    for(int i =0;i<=this->div[0];i++){
        painter.drawLine((pp[1].x - pp[0].x)*(double(i)/this->div[0]) + pp[0].x,
                         (pp[1].y - pp[0].y)*(double(i)/this->div[0]) + pp[0].y,
                         (pp[2].x - pp[3].x)*(double(i)/this->div[0]) + pp[3].x,
                         (pp[2].y - pp[3].y)*(double(i)/this->div[0]) + pp[3].y);//height);
    }
    for(int i =0;i<=this->div[0];i++){
        painter.drawLine((pp[2].x - pp[1].x)*(double(i)/this->div[0]) + pp[1].x,
                         (pp[2].y - pp[1].y)*(double(i)/this->div[0]) + pp[1].y,
                         (pp[3].x - pp[0].x)*(double(i)/this->div[0]) + pp[0].x,
                         (pp[3].y - pp[0].y)*(double(i)/this->div[0]) + pp[0].y);//height);
    }
}


QVector<Pos> CBlock::GetVerticesPos()const{
    //0~4で巡回するように

    QVector<Pos> pp;
    Pos old;
    pp.push_back(lines[0]->GetJointPos(0)); //先頭
    old = lines[0]->GetJointPos(0);

    //oldの相方を含むlineを探す
    for(int i=0;i<4;i++){
        if(lines[i]->GetJointPos(0) == old && !exist(pp,lines[i]->GetJointPos(1))){
            pp.push_back(lines[i]->GetJointPos(1));
            old = lines[i]->GetJointPos(1);
            i = 0;
        }else if(lines[i]->GetJointPos(1) == old && !exist(pp,lines[i]->GetJointPos(0))){
            pp.push_back(lines[i]->GetJointPos(0));
            old = lines[i]->GetJointPos(0);
            i = 0;
        }
    }

    return pp;
}
Pos CBlock::GetClockworksPos(int index)const{
    QVector<Pos> pp;
    //点を集結
    for(CObject* line:lines){
        if(!exist(pp,dynamic_cast<CLine*>(line)->GetJointPos(0)))pp.push_back(dynamic_cast<CLine*>(line)->GetJointPos(0));
        if(!exist(pp,dynamic_cast<CLine*>(line)->GetJointPos(1)))pp.push_back(dynamic_cast<CLine*>(line)->GetJointPos(1));
    }

    //左下の探索
    QVector<Pos> hit;
    Pos corner;//左下
    std::sort(pp.begin(),pp.end(),[](Pos p1,Pos p2){return std::tie(p1.x,p1.y) < std::tie(p2.x,p2.y);});//X座標が小さい順
    hit.push_back(pp[0]);//もっともX座標の小さいもの
    hit.push_back(pp[1]);//二番目にX座標の小さいもの
    std::sort(pp.begin(),pp.end(),[](Pos p1,Pos p2){return std::tie(p1.y,p1.x) < std::tie(p2.y,p2.x);});//Y座標が小さい順
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
        for(CObject* line:lines){
            if(ans == dynamic_cast<CLine*>(line)->GetJointPos(0) && old != dynamic_cast<CLine*>(line)->GetJointPos(1)){
                candidate.push_back(dynamic_cast<CLine*>(line)->GetJointPos(1));
            }else if(ans == dynamic_cast<CLine*>(line)->GetJointPos(1) && old != dynamic_cast<CLine*>(line)->GetJointPos(0)){
                candidate.push_back(dynamic_cast<CLine*>(line)->GetJointPos(0));
            }
        }
        //選定
        old = ans;
        if(candidate.size() == 2){
            //二択
            if(Pos::Angle(candidate[0]-corner,candidate[1]-corner) < Pos::Angle(candidate[0]-corner,candidate[1]-corner)){
                ans = candidate[0];
            }else{
                ans = candidate[1];
            }
        }else{
            ans = candidate[0];
        }
        candidate.clear();
    }
    return ans;
}


CBlock::CBlock()
{
}
CBlock::CBlock(QVector<CObject*> lines):
    lines(lines)
{
}

CBlock::~CBlock()
{

}

