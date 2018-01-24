#include "CLine.h"

CREATE_RESULT CLine::Create(CPoint *pos){
    if(this->start == nullptr){
        this->SetStartPos(pos);
        return CREATE_RESULT::ONESHOT;//あと1回
    }else{
        this->SetEndPos(pos);
        return CREATE_RESULT::COMPLETE;//完結
    }
}
void CLine::DrawGL(Pos camera,Pos center)const{
    //線の分割描画
    glBegin(GL_LINES);
    glVertex3f(this->GetMiddleDivide(0).x(),this->GetMiddleDivide(0).y(),this->GetMiddleDivide(0).z());
    glVertex3f(this->GetMiddleDivide(1).x(),this->GetMiddleDivide(1).y(),this->GetMiddleDivide(1).z());
    glEnd();
    DrawArrow(0.90,0.80);
    CEdge::DrawGL(camera,center);
}

bool CLine::isSelectable(Pos pos)const{
    //追加条件
    if(CObject::isSelectable(pos) &&
       std::min(this->start->x(),this->end->x()) - COLLISION_SIZE <= pos.x() &&
       std::max(this->start->x(),this->end->x()) + COLLISION_SIZE >= pos.x() &&
       std::min(this->start->y(),this->end->y()) - COLLISION_SIZE <= pos.y() &&
       std::max(this->start->y(),this->end->y()) + COLLISION_SIZE >= pos.y()){
        return true;
    }
    return false;
}


//中間点操作
CPoint*  CLine::GetPoint(int index){
    if(index == 0)return this->start;
    if(index == 1)return this->end;
    return nullptr;
}
CObject* CLine::GetChild(int index){
    return this->GetPoint(index);
}
CPoint*  CLine::GetPoint(int index)const{
    if(index == 0)return this->start;
    if(index == 1)return this->end;
    return nullptr;
}
CObject* CLine::GetChild(int index)const{
    return this->GetPoint(index);
}
void     CLine::SetChild(int index,CObject* obj){
    IgnoreChild(this->GetChild(index));
    if(index == 0)this->start = dynamic_cast<CPoint*>(obj);
    if(index == 1)this->end   = dynamic_cast<CPoint*>(obj);
    ObserveChild(this->GetChild(index));
}

int CLine::GetChildCount()const{
    return 2;
}
Pos  CLine::GetMiddleDivide(double t)const{
    return (*end - *start) * t + *start;
}

Pos CLine::GetNearPos(const Pos& hand)const{
    //点と直線の最近点
    return Pos::LineNearPoint(*this->start,*this->end,hand);
}
Pos CLine::GetNearLine(const Pos& pos1,const Pos& pos2)const{
    //pos1とpos2に投影
    Pos dir = (pos2-pos1).GetNormalize();
    Pos start_ = *this->start - dir * dir.DotPos(*this->start);
    Pos end_   = *this->end   - dir * dir.DotPos(*this->end);

    //点と直線の最近点
    Pos start_end_dir = (*this->end - *this->start).GetNormalize();
    Pos near = Pos::LineNearPoint(start_,end_,pos1);

    //判定
    if(( start_end_dir).DotPos(near - end_  ) > 0)return *this->end;//end方向はみ出し
    if((-start_end_dir).DotPos(near - start_) > 0)return *this->start;//start方向はみ出し
    return near;
}


CEdge* CLine::Clone()const{
    CLine* ptr = new CLine(this->parent());
    for(int i= 0;i<this->GetChildCount();i++){
        ptr->SetChild(i,this->GetChild(i));
    }
    ptr->grading = this->grading;
    ptr->divide  = this->divide;
    return ptr;
}

//結合
CEdge*          CLine::MergeEdge (CEdge* merge){
    //型が同じでない
    if(!merge->is<CLine>()){
        QMessageBox::critical(nullptr, tr("結合エラー"), tr("型が違います。同一タイプの線を選択してください。"));
        return nullptr;
    }
    //一致しない
    CPoint* merge_point;
    if(this->start == merge->start)merge_point = this->start;
    if(this->start == merge->end  )merge_point = this->start;
    if(this->end   == merge->start)merge_point = this->end;
    if(this->end   == merge->end  )merge_point = this->end;
    if(merge_point == nullptr){
        QMessageBox::critical(nullptr, tr("結合エラー"), tr("二つの線の始点または終点が一致している必要があります。"));
        return nullptr;
    }

    //方向が違う
    /*
    if(std::abs((((*this ->end) - (*this ->start)).GetNormalize()).DotPos
                (((*merge->end) - (*merge->start)).GetNormalize())) !=  1){
        QMessageBox::critical(nullptr, tr("結合エラー"), tr("二つの線の方向が一致している必要があります。"));
        return nullptr;
    }*/

    //作成
    CLine* new_line = new CLine();
    new_line->Create(this ->start == merge_point ? this ->end : this ->start);
    new_line->Create(merge->start == merge_point ? merge->end : merge->start);
    return new_line;
}
QList<CEdge*> CLine::DivideEdge(CPoint* division){
    CEdge* edge_s = new CLine();
    CEdge* edge_e = new CLine();
    edge_s->start = this->start;
    edge_s->end   = division;
    edge_e->start = division;
    edge_e->end   = this->end;
    return {edge_s,edge_e};
}


CLine::CLine(QObject *parent):
    CEdge(parent)
{
}

CLine::CLine(CPoint* start,CPoint* end,QObject* parent):
    CEdge(parent){
    this->SetChild(0,start);
    this->SetChild(1,end);
}
CLine::~CLine(){}
