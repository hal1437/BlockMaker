#include "CObject.h"

double   CObject::drawing_scale;
QVector<CObject*> CObject::selected;

bool CObject::isSelecting()const{
    return false;//(hanged == this);
}
bool CObject::isCreating()const{
    return false;//(creating == this);
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
    return (this->GetNear(pos) - pos).Length2D() < CObject::COLLISION_SIZE / drawing_scale;
}

CObject::CObject(QObject* parent):QObject(parent)
{

}

CObject::~CObject()
{

}

