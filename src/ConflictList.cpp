#include "ConflictList.h"

//モデルの競合を自動的に追加
void ConflictList::SetModel(CadModelCore *m){
    this->model = m;
    connect(this->model,SIGNAL(ConflictAnyObject(CObject*,Conflict)),this,SLOT(AddConflict(CObject*,Conflict)));
}

void ConflictList::AddConflict(CObject *obj, Conflict conf){
    this->conflicts.push_back(std::make_pair(obj,conf));
}

ConflictList::ConflictList()
{

}
