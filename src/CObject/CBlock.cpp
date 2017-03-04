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
            this->lines.push_back(dynamic_cast<CRect*>(lines[0])->GetLines()[i]);
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

void CBlock::Draw(QPainter& painter, QTransform trans)const{
    //描画範囲算出
    QVector<Pos> pp = this->GetVerticesPos();

    //全て変換
    for(Pos& p:pp){
        p.Transform(trans);
    }

    float top,bottom,left,right;
    top    = std::min_element(pp.begin(),pp.end(),[](const Pos& lhs,const Pos& rhs){return lhs.y < rhs.y;})->y;
    bottom = std::max_element(pp.begin(),pp.end(),[](const Pos& lhs,const Pos& rhs){return lhs.y < rhs.y;})->y;
    left   = std::min_element(pp.begin(),pp.end(),[](const Pos& lhs,const Pos& rhs){return lhs.x < rhs.x;})->x;
    right  = std::max_element(pp.begin(),pp.end(),[](const Pos& lhs,const Pos& rhs){return lhs.x < rhs.x;})->x;


    //パスの作成
    QPainterPath myPath;
    myPath.moveTo(pp[0].x,pp[0].y);
    for(int i=0;i<4+1;i++){
        Pos p =  pp[i%4];
        myPath.lineTo(p.x,p.y);
    }
    myPath.closeSubpath();

    //マスクを作成
    QImage mask(QSize(right+20,bottom+20), QImage::Format_ARGB32);
    QPainter paint;// = new QPainter(mask);
    mask.fill(0);
    paint.begin(&mask);
    paint.setCompositionMode(QPainter::CompositionMode_SourceOver);
    paint.setPen  (painter.pen());
    paint.setBrush(painter.brush());
    paint.drawPath(myPath);
    paint.setCompositionMode(QPainter::CompositionMode_SourceAtop);
    for(int i =0;i<=this->grading_args[0];i++){
        paint.drawLine(left ,top+(bottom-top)*i/this->grading_args[0],
                       right,top+(bottom-top)*i/this->grading_args[0]);
    }
    //Y軸に並行
    for(int i =0;i<=this->grading_args[1];i++){
        paint.drawLine(left+(right-left)*i/this->grading_args[1],top,
                       left+(right-left)*i/this->grading_args[1],bottom);
    }
    paint.end();

    painter.drawImage(QPoint(0,0),mask);
    /*

    //マクス適用
    painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);


    //分割して線を描画
    //X軸に並行
    for(int i =0;i<this->grading_args[0]-1;i++){
        painter.drawLine(left ,top+(bottom-top)* i   /this->grading_args[0],
                         right,top+(bottom-top)*(i+1)/this->grading_args[0]);
    }
    //Y軸に並行
    for(int i =0;i<this->grading_args[1]-1;i++){
        painter.drawLine(left+(right-left)* i   /this->grading_args[1],top,
                         left+(right-left)*(i+1)/this->grading_args[1],bottom);
    }*/
}


QVector<Pos> CBlock::GetVerticesPos()const{
    QVector<Pos> pp;
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

