#ifndef RESTRAINT_H
#define RESTRAINT_H
#include <algorithm>
#include <vector>
#include "Point.h"

//拘束
struct Restraint{
    Pos center; //位置
    Pos dir;    //方向

    //最寄りの拘束点を取得
    virtual Pos GetNearPoint(const Pos& pos)const = 0;

    //いつもの
    template<class T>
    bool is()const {
        return (dynamic_cast<const T*>(this) != nullptr);
    }

    Restraint(){}
    Restraint(const Pos& Center,const Pos& Dir):center(Center),dir(Dir){}
    virtual ~Restraint(){}
};

#define RESTRAINT_MAKE_DEF(CLASS_NAME)                      \
struct CLASS_NAME : public Restraint{                       \
                                                            \
    virtual Pos GetNearPoint(const Pos& pos)const;          \
                                                            \
    CLASS_NAME(){}                                          \
    CLASS_NAME(const Pos& c,const Pos& d):Restraint(c,d){}  \
    virtual ~CLASS_NAME(){};                                \
};                                                          \

RESTRAINT_MAKE_DEF(LineRestraint     ) //線形拘束
RESTRAINT_MAKE_DEF(ArcRestraint      ) //円弧拘束
RESTRAINT_MAKE_DEF(FixRestraint      ) //固定拘束
RESTRAINT_MAKE_DEF(ParadoxRestraint  ) //矛盾拘束

//合成拘束
struct CompositRestraint : public Restraint{
    std::vector<const Restraint*> ref;//参照先

    virtual Pos GetNearPoint(const Pos& pos)const;
    CompositRestraint(){}
    CompositRestraint(const Pos& c,const Pos& d):Restraint(c,d){}
    virtual ~CompositRestraint(){}
};

//演算子定義
CompositRestraint operator|(const Restraint& lhs  ,const Restraint& rhs); //or演算
Restraint*        operator&(const Restraint& lhs  ,const Restraint& rhs); //and演算

#endif // RESTRAINT_H
