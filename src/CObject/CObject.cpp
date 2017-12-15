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
QVector<CPoint *> CObject::GetAllChildren()const{
    QVector<CPoint*> ans;
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


void CObject::ObservePause  (){
    this->observe_pause = true;
}
void CObject::ObserveRestart(){
    this->observe_pause = false;
    if(this->observe_queue.size() > 0){
        //更新
        this->ChangeChildCallback(this->observe_queue);
        this->observe_queue.clear();
    }
}


CObject::CObject(QObject* parent):QObject(parent)
{
    observe_pause = false;
}

CObject::~CObject(){}


void CObject::StackChangeCallback(CObject* child){
    if(this->observe_pause == true){
         this->observe_queue.push_back(child);
    }else{
        this->ChangeChildCallback({child});
    }
}
void CObject::ChangeChildCallback(QVector<CObject*>){
}
