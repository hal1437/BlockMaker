#include "CObject.h"

CObject* CObject::select_obj;
Pos CObject::mouse_over;


void CObject::SetSelecting(bool f){
    this->selecting = f;
}

bool CObject::Make(Pos& pos,int index){
    Relative<Pos> r;
    r = pos;
    Create(r,index);
    return true;
}

bool CObject::Make(Pos&& pos, int index){
    Relative<Pos> r;
    r = std::move(pos);
    Create(r,index);
    return true;
}
bool CObject::Make(Relative<Pos>& pos, int index){
    Relative<Pos> r;
    r.setRelative(&pos);
    Create(r,index);
    return true;
}

CObject::CObject()
{

}

CObject::~CObject()
{

}

