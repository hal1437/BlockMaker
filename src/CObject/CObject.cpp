#include "CObject.h"

double   CObject::drawing_scale;

void CObject::Lock(bool flag){
    this->lock = flag;
}

bool CObject::isLock()const{
    return this->lock;
}

void CObject::Visible(bool flag){
    this->visible = flag;
}

bool CObject::isVisible()const{
    return this->visible;
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

