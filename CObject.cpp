#include "CObject.h"

CObject* CObject::selecting;
std::vector<CObject*> CObject::selected;
Pos CObject::mouse_over;


bool CObject::isCreateing()const{
    return is_Creating;
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
    return Create(r,index);
}
bool CObject::Make(Relative<Pos>& pos, int index){
    Relative<Pos> r;
    r.setReference(&pos);
    return Create(r,index);
}

CObject::CObject()
{

}

CObject::~CObject()
{

}

