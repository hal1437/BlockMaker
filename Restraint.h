#ifndef RESTRAINT_H
#define RESTRAINT_H
#include <algorithm>
#include <vector>
#include "Point.h"
#include "CObject.h"
#include "CLine.h"
#include "CArc.h"
#include "CSpline.h"


enum RestraintType{
    EQUAL      ,//等しい値
    VERTICAL   ,//垂直拘束 c:[-.]
    HORIZONTAL ,//水平拘束 c:[-.]
    MATCH      ,//一致拘束 c:[p.-]
    CONCURRENT ,//並行拘束 c:[l,l]
    CROSS      ,
    ANGLE      ,//角度拘束 c:[l,l]
    TANGENT    ,//正接拘束 c:[l]
    FIX        ,//固定拘束 c:[]
    Paradox    ,//矛盾拘束 c:[]
    MARGE      ,
};

//拘束
struct Restraint{
    std::vector<CObject*> nodes;//拘束対象
    double value;      //値
    RestraintType type; //タイプ

    static std::vector<RestraintType> Restraintable(const std::vector<CObject*>& values);

    //最寄りの点に補完
    virtual bool Complete() = 0;

    //いつもの
    template<class T>
    bool is()const{
        return (dynamic_cast<const T*>(this) != nullptr);
    }

    Restraint(){}
    Restraint(std::vector<CObject*> Nodes,
              double Value):
        nodes(Nodes),
        value(Value){}

    virtual ~Restraint(){}
};

#define RESTRAINT_MAKE_DEF(CLASS_NAME)                      \
struct CLASS_NAME : public Restraint{                       \
                                                            \
    bool Complete();                                        \
                                                            \
    CLASS_NAME();                                           \
    CLASS_NAME(std::vector<CObject*> Nodes,                 \
              double Value=0):                                \
        Restraint(Nodes,Value){CLASS_NAME();};              \
    virtual ~CLASS_NAME(){};                                \
};                                                          \

RESTRAINT_MAKE_DEF(EqualRestraint     ) //等値
RESTRAINT_MAKE_DEF(VerticalRestraint  ) //垂直拘束 c:[l] s:[]
RESTRAINT_MAKE_DEF(HorizontalRestraint) //水平拘束 c:[l] s:[]
RESTRAINT_MAKE_DEF(MatchRestraint     ) //一致拘束 c:[p] s:[-]
RESTRAINT_MAKE_DEF(ConcurrentRestraint) //並行拘束 c:[l,l] s:[]
RESTRAINT_MAKE_DEF(CrossRestraint       ) //拘束 c:[]  s:[]
//RESTRAINT_MAKE_DEF(TangentRestraint   ) //正接拘束 c:[l] s:[a]
RESTRAINT_MAKE_DEF(FixRestraint       ) //固定拘束 c:[]  s:[]
RESTRAINT_MAKE_DEF(ParadoxRestraint   ) //矛盾拘束 c:[]  s:[]



//演算子定義
Restraint*        operator&(const Restraint& lhs  ,const Restraint& rhs); //and演算

#endif // RESTRAINT_H
