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
    ptr->SetStartPos(new CPoint(*this->start,ptr));
    ptr->SetEndPos  (new CPoint(*this->end  ,ptr));
    ptr->grading = this->grading;
    ptr->divide  = this->divide;
    return ptr;
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
