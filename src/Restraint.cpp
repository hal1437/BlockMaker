#include "Restraint.h"

void Restraint::ObserveChild(CObject* obj){
    //コールバック接続
    if(obj != nullptr){
        connect(obj,SIGNAL(Changed(CObject*)),this,SLOT(ChangeObjectCallback(CObject*)));
    }
}
void Restraint::IgnoreChild(CObject* obj){
    //コールバック接続解除
    if(obj != nullptr){
        disconnect(obj,SIGNAL(Changed(CObject*)),this,SLOT(ChangeObjectCallback(CObject*)));
    }
}

QVector<RestraintType> Restraint::Restraintable(const QVector<CObject*> nodes){
    QVector<RestraintType> ans;
    if(EqualLengthRestraint::Restraintable(nodes))ans.push_back(RestraintType::EQUAL);
    return ans;
}
void Restraint::Create(const QVector<CObject*> nodes, double value){
    this->nodes = nodes;
    this->setValue(value);
}

bool EqualLengthRestraint::Restraintable(const QVector<CObject *> nodes){
    if(nodes.size()<2)return false;
    //全てCedgeであれば
    return std::all_of(nodes.begin(),nodes.end(),[](CObject* obj){return obj->is<CEdge>();});
}
void EqualLengthRestraint::Calc(){
    //先頭のCEdgeの長さに統一する。
    double dd = (*dynamic_cast<CEdge*>(this->nodes[0])->end - *dynamic_cast<CEdge*>(this->nodes[0])->start).Length();
    for(int i=1;i<this->nodes.size();i++){
        CEdge* edge = dynamic_cast<CEdge*>(this->nodes[i]);
        //endを移動させる
        *edge->end = (*edge->end - *edge->start).GetNormalize() * dd + *edge->start;
    }
}
void EqualLengthRestraint::ChangeObjectCallback(CObject* obj){
    //変更されたエッジを先頭に出す
    for(int i=1;i< this->nodes.size();i++){
        if(this->nodes[i] == obj){
            std::swap(this->nodes[0],this->nodes[i]);
            break;
        }
    }
    //再計算
    this->Calc();
}
