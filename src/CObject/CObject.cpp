#include "CObject.h"

double   CObject::drawing_scale;

void CObject::ObserveChild(CObject* obj){
    //コールバック接続
    if(obj != nullptr){
        connect(obj,SIGNAL(Changed(CObject*)),this,SLOT(StackChangeCallback(CObject*)));
    }
}

void CObject::IgnoreChild(CObject* obj){
    //コールバック接続解除
    if(obj != nullptr){
        disconnect(obj,SIGNAL(Changed(CObject*)),this,SLOT(StackChangeCallback(CObject*)));
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
QList<CPoint *> CObject::GetAllChildren()const{
    QList<CPoint*> ans;
    for(int i=0;i<this->GetChildCount();i++){
        //追加
        if(this->GetChild(i) != nullptr){
            for(CPoint* v:this->GetChild(i)->GetAllChildren()){
                ans.push_back(v);
            }
        }
    }
    return ans;
}

void CObject::Refresh(){
}

void CObject::ObserveIgnore    (){
    this->observe_status = OBSERVE_STATUS::IGNORE;
}
void CObject::ObserveRestart   (){
    this->observe_status = OBSERVE_STATUS::OBSERVE;
}
void CObject::ObserveStack(){
    this->observe_status = OBSERVE_STATUS::STACK;
}
void CObject::ObservePop(){
    if(this->observe_stack.size() > 0){
        //止めていた分をまとめて更新
        this->ChangeChildCallback(this->observe_stack);
        this->observe_stack.clear();
    }
    this->observe_status = OBSERVE_STATUS::OBSERVE;
}


CObject::CObject(QObject* parent):QObject(parent)
{
    this->observe_status = OBSERVE_STATUS::OBSERVE;
}

CObject::~CObject(){}


void CObject::StackChangeCallback(CObject* child){
    if(this->observe_status == OBSERVE_STATUS::OBSERVE){
        this->ChangeChildCallback({child});
    }
    if(this->observe_status == OBSERVE_STATUS::STACK){
        this->observe_stack.push_back(child);
    }
}
void CObject::ChangeChildCallback(QList<CObject*>){
}
