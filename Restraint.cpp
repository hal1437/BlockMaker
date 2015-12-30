#include "Restraint.h"


Pos LineRestraint::GetNearPoint(const Pos& pos)const{
    return Pos::LineNearPoint(center,center+dir,pos);
}
Pos ArcRestraint::GetNearPoint(const Pos& pos)const{
    return Pos::CircleNearPoint(center,dir.Length(),pos);
}
Pos FixRestraint::GetNearPoint(const Pos&)const{
    return center;
}
Pos ParadoxRestraint::GetNearPoint(const Pos&)const{
    return Pos();
}
Pos CompositRestraint::GetNearPoint(const Pos& pos)const{
    return (*std::min_element(ref.begin(),ref.end(),[&](const Restraint* lhs,const Restraint* rhs){
        return ((lhs->GetNearPoint(pos)-pos).Length() < (rhs->GetNearPoint(pos)-pos).Length());
    }))->GetNearPoint(pos);
}

CompositRestraint operator|(const Restraint& lhs  ,const Restraint& rhs){
    CompositRestraint answer;
    answer.ref.push_back(&lhs);
    answer.ref.push_back(&rhs);
    return answer;
}
Restraint* operator&(const Restraint& l,const Restraint& r){
    Restraint* rest = nullptr;
    const Restraint* lhs = &l;
    const Restraint* rhs = &r;
    const double MIN_LIM  = 0.1; //誤差

    //固定が存在
    if(!lhs->is<FixRestraint>() && rhs->is<FixRestraint>())std::swap(lhs,rhs);
    if(lhs->is<FixRestraint>()){
        //両固定
        if(rhs->is<FixRestraint>()){
            if(lhs->center == rhs->center)rest = new FixRestraint(lhs->center,lhs->dir);
        }
        //他、とにかく線上に乗っていればおk
        if(rhs->is<LineRestraint>()){
            if(std::abs((rhs->GetNearPoint(lhs->center) - lhs->GetNearPoint(Pos())).Length()) < MIN_LIM)rest = new FixRestraint(lhs->center,lhs->dir);
        }
    }
    //線
    if(!lhs->is<LineRestraint>() && rhs->is<LineRestraint>())std::swap(lhs,rhs);
    if(lhs->is<LineRestraint>()){
        //両線
        if(rhs->is<FixRestraint>()){
            if(lhs->center == rhs->center)rest = new FixRestraint(lhs->center,lhs->dir);
            //二直線の交点のアルゴリズム
            //else
        }
        //他、とにかく線上に乗っていればおk
        if(rhs->is<LineRestraint>()){
            if(std::abs((rhs->GetNearPoint(lhs->center) - lhs->GetNearPoint(Pos())).Length()) < MIN_LIM)rest = new FixRestraint(lhs->center,lhs->dir);
        }
    }

    if(rest==nullptr)rest = new ParadoxRestraint();
    return rest;
}

