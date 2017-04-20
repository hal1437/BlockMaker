#include "CObject.h"

double   CObject::drawing_scale;
CObject* CObject::hanged;
CObject* CObject::creating;
QVector<CObject*> CObject::selected;
Pos CObject::mouse_pos;

bool CObject::isSelecting()const{
    return (hanged == this);
}
bool CObject::isCreating()const{
    return (creating == this);
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
bool CObject::isSelectable(Pos pos)const{
    if(this->isCreating())return false;
    return (this->GetNear(pos) - pos).Length() < CObject::COLLISION_SIZE;
}

CObject::CObject(QObject* parent):QObject(parent)
{

}

CObject::~CObject()
{

}

