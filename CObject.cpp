#include "CObject.h"

CObject* CObject::selecting;
QVector<CObject*> CObject::selected;
Pos CObject::mouse_over;


bool CObject::isCreateing()const{
    return is_Creating;
}

bool CObject::Make(CPoint* pos,int index){
    return Create(pos,index);
}

CObject::CObject()
{

}

CObject::~CObject()
{

}

