#include "CObject.h"

double   CObject::drawing_scale;


void CObject::ObserveChild(CObject* obj){
    //コールバック接続
    if(obj != nullptr){
        connect(obj,SIGNAL(Moved(CObject*)),this,SLOT(ChangeChildHandler(CObject*)));
    }
}

void CObject::IgnoreChild(CObject* obj){
    //コールバック接続解除
    if(obj != nullptr){
        disconnect(obj,SIGNAL(Moved(CObject*)),this,SLOT(ChangeChildHandler(CObject*)));
    }
}


/*
void CObject::MoveAbsolute(const Pos& diff){
    QVector<CPoint*> children = this->GetAllChildren();
    Pos sum;//平均値
    for(CPoint* p : children){
        sum = sum + *reinterpret_cast<Pos*>(p);
    }
    sum /= children.size();

    //平均値との差分だけ移動
    for(CPoint*& p : children){
        p->MoveRelative(sum - diff);
    }
}
void CObject::MoveRelative(const Pos& diff){
    QVector<CPoint*> children = this->GetAllChildren();
    for(CPoint*& p : children){
        p->MoveRelative(diff);
    }
}*/
bool CObject::isSelectable(Pos pos)const{
    return Pos(this->GetNearPos(pos) - pos).Length() < CObject::COLLISION_SIZE / drawing_scale;
}

CObject*  CObject::GetChild     (int index)const{
    return const_cast<CObject*>(this)->GetChild(index);
}
void CObject::InsertChild  (int index,CObject* obj){
    //押し出し
    for(int i=this->GetChildCount()-1;i>index;i--){
        this->SetChild(index,this->GetChild(index-1));
    }
    this->SetChild(index,obj);
}
QVector<CPoint *> CObject::GetAllChildren()const{
    QVector<CPoint*> ans;
    for(int i=0;i<this->GetChildCount();i++){
        //追加
        for(CPoint* v:this->GetChild(i)->GetAllChildren())ans.push_back(v);
    }
    return ans;
}

CObject::CObject(QObject* parent):QObject(parent)
{

}

CObject::~CObject()
{

}

void CObject::ChangeChildCallback(CObject*){
}

void CObject::ChangeChildHandler(CObject* obj){
    ChangeChildCallback(obj);
    emit Moved();
}

