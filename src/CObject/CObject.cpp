#include "CObject.h"

double   CObject::drawing_scale;
CObject* CObject::hanged;
CObject* CObject::createing;
QVector<CObject*> CObject::selected;
Pos CObject::mouse_pos;

bool CObject::isSelecting()const{
    return (hanged == this);
}
bool CObject::isCreating()const{
    return (createing == this);
}
bool CObject::isSelected()const{
    return exist(selected,this);
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

