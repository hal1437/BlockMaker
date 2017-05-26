#include "CObject.h"

double   CObject::drawing_scale;

void CObject::Lock(bool lock){
    this->lock = lock;
}

bool CObject::isLock()const{
    return this->lock;
}
bool CObject::isSelectable(Pos pos)const{
    return Pos(this->GetNearPos(pos) - pos).Length() < CObject::COLLISION_SIZE / drawing_scale;
}


CObject::CObject(QObject* parent):QObject(parent)
{

}

CObject::~CObject()
{

}

