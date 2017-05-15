#include "CEdge.h"

void CEdge::SetStartPos(CPoint* pos){
    disconnect(this->start,SIGNAL(PosChanged(Pos,Pos)),this,SLOT(ChangePosCallback(Pos,Pos)));
    this->start = pos;
    connect(this->start,SIGNAL(PosChanged(Pos,Pos)),this,SLOT(ChangePosCallback(Pos,Pos)));
}

void CEdge::SetEndPos(CPoint* pos){
    disconnect(this->end,SIGNAL(PosChanged(Pos,Pos)),this,SLOT(ChangePosCallback(Pos,Pos)));
    this->end = pos;
    connect(this->end,SIGNAL(PosChanged(Pos,Pos)),this,SLOT(ChangePosCallback(Pos,Pos)));
}

int CEdge::GetPosSequenceCount()const{
    return this->GetMiddleCount()+2;
}
CPoint* CEdge::GetPosSequence(int index)const{
    if     (index == 0                        )return this->start;
    else if(index == GetPosSequenceCount() - 1)return this->end;
    else if(index > 0 && index < GetPosSequenceCount() - 1)return this->GetMiddle(index-1);
}


CEdge::CEdge(QObject* parent):
    CObject(parent)
{
    this->start = this->end = nullptr;
}

CEdge::~CEdge()
{
}

//点移動コールバック
void CEdge::ChangePosCallback(const Pos& new_pos,const Pos& old_pos){
}

