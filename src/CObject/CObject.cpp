#include "CObject.h"

double   CObject::drawing_scale;


void CObject::MoveAbsolute(const Pos& diff){
    QVector<CObject*> children = this->GetAllChildren();
    Pos sum;//平均値
    for(CObject* p : children){
        sum = sum + *reinterpret_cast<Pos*>(p);
    }
    sum /= children.size();

    //平均値との差分だけ移動
    for(CObject*& p : children){
        p->MoveRelative(sum - diff);
    }
}
void CObject::MoveRelative(const Pos& diff){
    QVector<CObject*> children = this->GetAllChildren();
    for(CObject*& p : children){
        p->MoveRelative(diff);
    }
}
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
QVector<CObject*> CObject::GetAllChildren()const{
    QVector<CObject*> ans;
    for(int i=0;i<this->GetChildCount();i++){
        QVector<CObject*> add = this->GetChild(i)->GetAllChildren();
        //追加
        for(CObject* v:add)ans.push_back(v);
    }
    return ans;
}



CObject::CObject(QObject* parent):QObject(parent)
{

}

CObject::~CObject()
{

}

