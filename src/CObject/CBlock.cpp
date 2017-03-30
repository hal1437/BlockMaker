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
    QVector<Pos> pp = this->GetVerticesPos();
    top    = std::min_element(pp.begin(),pp.end(),[](const Pos& lhs,const Pos& rhs){return lhs.y < rhs.y;})->y;
    bottom = std::max_element(pp.begin(),pp.end(),[](const Pos& lhs,const Pos& rhs){return lhs.y < rhs.y;})->y;
    left   = std::min_element(pp.begin(),pp.end(),[](const Pos& lhs,const Pos& rhs){return lhs.x < rhs.x;})->x;
    right  = std::max_element(pp.begin(),pp.end(),[](const Pos& lhs,const Pos& rhs){return lhs.x < rhs.x;})->x;

    //パスの作成
    QPainterPath myPath;
    myPath.moveTo(pp[pp.size()-1].x- left,pp[pp.size()-1].y -top);
    for(int i=0;i<pp.size();i++){
        myPath.lineTo(pp[i].x - left,pp[i].y-top);
    }
    myPath.closeSubpath();

    //マスクを作成
    double width  = right  - left;
    double height = bottom - top;
    QImage mask(QSize(width,height), QImage::Format_ARGB32);
    QPainter paint;// = new QPainter(mask);
    mask.fill(0);
    paint.begin(&mask);
    paint.setRenderHint(QPainter::Antialiasing, true);//アンチエイリアスセット
    paint.setCompositionMode(QPainter::CompositionMode_SourceOver);
    paint.setPen  (painter.pen());
    paint.setBrush(painter.brush());
    paint.drawPath(myPath);
    paint.setCompositionMode(QPainter::CompositionMode_SourceAtop);


    //並行線の描画
    for(int i =0;i<=this->div[0];i++){
        paint.drawLine(width*i/this->div[0],0,
                       width*i/this->div[0],height);
    }
    for(int i =0;i<=this->div[1];i++){
        paint.drawLine(0    ,height*i/this->div[1],
                       width,height*i/this->div[1]);
    }
    paint.end();

    painter.drawImage(QPoint(left,top),mask);
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
Pos CBlock::GetClockworksPos(int index){

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

