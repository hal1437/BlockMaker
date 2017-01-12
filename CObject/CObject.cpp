#include "CObject.h"

CObject* CObject::selecting;
CObject* CObject::createing;
QVector<CObject*> CObject::selected;
Pos CObject::mouse_over;

bool CObject::isSelecting()const{
    return (selecting == this);
}
bool CObject::isCreating()const{
    return (createing == this);
}

bool CObject::isSelected()const{
    return exist(selected,this);
}


bool CObject::Make(CPoint* pos,int index){
    return Create(pos,index);
}

void CObject::Lock(bool lock){
    this->lock = lock;
}

bool CObject::isLock()const{
    return this->lock;
}


CObject::CObject()
{

}

CObject::~CObject()
{

}

