#include "CEdge.h"

void CEdge::SetStartPos(CPoint* pos){
    if(this->start != nullptr){
        disconnect(this->start,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
    }
    this->start = pos;
    connect   (this->start,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
}

void CEdge::SetEndPos(CPoint* pos){
    if(this->end != nullptr){
        disconnect(this->end,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
    }
    this->end = pos;
    connect(this->end,SIGNAL(PosChanged(CPoint*,Pos)),this,SLOT(ChangePosCallback(CPoint*,Pos)));
}

int CEdge::GetPosSequenceCount()const{
    return this->GetMiddleCount()+2;
}
CPoint* CEdge::GetPosSequence(int index)const{
    if     (index == 0                        )return this->start;
    else if(index == GetPosSequenceCount() - 1)return this->end;
    else if(index > 0 && index < GetPosSequenceCount() - 1)return this->GetMiddle(index-1);
    return nullptr;
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
void CEdge::ChangePosCallback(CPoint* ,Pos ){
}

