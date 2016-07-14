#include "CBlocks.h"

bool CBlocks::Creatable(std::vector<CObject*> values){
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
    return false;
}

void CBlocks::SetNodeAll(std::vector<CObject*> lines){
    this->lines = lines;
}
void CBlocks::SetNode(int index,CObject* line){
    this->lines[index] = line;
}

CObject* CBlocks::GetNode(int index)const{
    return this->lines[index];
}

void CBlocks::Draw(QPainter& painter)const{

    //パスの作成
    QPainterPath myPath;
    QPolygonF myPolygon;
    for(int i=0;i<4+1;i++){
        Pos p =  this->GetVerticesPos()[i%4];
        myPolygon.push_back(QPointF(p.x,p.y));
    }
    myPath.addPolygon(myPolygon);
    myPath.setFillRule(Qt::OddEvenFill);

    //マスクを作成
    QPixmap *mask = new QPixmap(500,500);
    QPainter *paint = new QPainter(mask);
    paint->setPen(*(new QColor(255,255,255,255)));
    painter.drawPath(myPath);

}


std::vector<Pos> CBlocks::GetVerticesPos()const{
    std::vector<Pos> pp;
    CObject* old = lines[0];
    pp.push_back(lines[0]->GetJointPos(0));
    pp.push_back(lines[0]->GetJointPos(lines[0]->GetJointNum()-1));
    //連結を探索
    for(int i=0;lines[0]->GetJointPos(0) != pp[pp.size()-1];i++){
        if(lines[i%4] == old)continue;
        Pos p1 = lines[i%4]->GetJointPos(0);
        Pos p2 = lines[i%4]->GetJointPos(lines[i%4]->GetJointNum()-1);
        if(p1 == pp[pp.size()-1]){
            pp.push_back(p2);
            old=lines[i%4];
        }else if(p2 == pp[pp.size()-1]){
            pp.push_back(p1);
            old=lines[i%4];
        }
    }
    return pp;
}

CBlocks::CBlocks()
{
}
CBlocks::CBlocks(std::vector<CObject*> lines):
    lines(lines)
{
}

CBlocks::~CBlocks()
{

}

