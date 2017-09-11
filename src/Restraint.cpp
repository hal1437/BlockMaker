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
    if( ConcurrentRestraint::Restraintable(nodes))ans.push_back(RestraintType::CONCURRENT);
    return ans;
}
void Restraint::Create(const QVector<CObject*> nodes, double value){
    this->nodes = nodes;
    for(CObject* edge:nodes)ObserveChild(edge);
    this->setValue(value);
}

QVector<Pos> Restraint::GetIconPoint()const{
    QVector<Pos> ans;
    for(CObject* obj:this->nodes){
        Pos p;
        if(obj->is<CPoint>())p = *dynamic_cast<CPoint*>(obj);//点自身
        else if(obj->is<CEdge>() )p = dynamic_cast<CEdge*>(obj)->GetMiddleDivide(0.5);//中点
        else {
            QVector<CPoint*> children = obj->GetAllChildren();
            for(CPoint* pos:children){
                p += *pos;
            }
            p /= children.size();
        }
        ans.push_back(p);
    }
    return ans;
}


bool EqualLengthRestraint::isComplete(){
    double dd = (*dynamic_cast<CEdge*>(this->nodes[0])->end - *dynamic_cast<CEdge*>(this->nodes[0])->start).Length();
    return std::all_of(this->nodes.begin()+1,this->nodes.end(),[dd](CObject* obj){
        CEdge* edge = dynamic_cast<CEdge*>(obj);
        return (*edge->end - *edge->start).Length() == dd;
    });
}
bool ConcurrentRestraint::isComplete(){
    Pos pos = (*dynamic_cast<CEdge*>(this->nodes[0])->end - *dynamic_cast<CEdge*>(this->nodes[0])->start).GetNormalize();
    return std::all_of(this->nodes.begin()+1,this->nodes.end(),[pos](CObject* obj){
        CEdge* edge = dynamic_cast<CEdge*>(obj);
        return (*edge->end - *edge->start).GetNormalize() == pos;
    });
}

void EqualLengthRestraint::Calc(){
    //先頭のCEdgeの長さに統一する。
    bool changed = false;
    double dd = (*dynamic_cast<CEdge*>(this->nodes[0])->end - *dynamic_cast<CEdge*>(this->nodes[0])->start).Length();
    for(int i=1;i<this->nodes.size();i++){
        CEdge* edge = dynamic_cast<CEdge*>(this->nodes[i]);
        //中心へ移動させる
        Pos center = (*edge->end + *edge->start)/2;
        *edge->start = (*edge->start - center).GetNormalize() * dd/2 + center;
        *edge->end   = (*edge->end   - center).GetNormalize() * dd/2 + center;
        changed = true;
    }
    if(changed){
        //emit Changed();
    }
}

void ConcurrentRestraint::Calc(){
    //先頭のCEdgeと同じ向きに変更する。
    bool changed = false;
    Pos base = (*dynamic_cast<CEdge*>(this->nodes[0])->end - *dynamic_cast<CEdge*>(this->nodes[0])->start).GetNormalize();
    for(int i=1;i<this->nodes.size();i++){
        CEdge* edge = dynamic_cast<CEdge*>(this->nodes[i]);
        //endを移動させる
        double l = (*edge->end  - *edge->start).Length();
        *edge->end = base * l + *edge->start;
        changed = true;
    }
    if(changed){
        //emit Changed();
    }
}
