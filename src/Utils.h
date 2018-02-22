#ifndef UTILS
#define UTILS

#include "Algorithm.h"
#define PI M_PI
#define TAB "    "
#define NEWLINE "\n"
#define MOUSE_ZOOM_RATE 10000.0
#define NearlyEqual(RHS,LHS) (std::abs(RHS-LHS) < 0.000001)

//エミッター:シグナルを発生させる関数
#define DEFINE_EMITTOR(NAME)        \
void NAME##Emittor(){               \
    emit NAME();                    \
}
#define DEFINE_EMITTOR_ARG(NAME,ARG) \
void NAME##Emittor(){           \
    emit NAME(ARG);             \
}

//シンクロナイズ : シグナル発生時に別のシグナルを発生させる(要エミッター)
#define SYNCHRONIZE_SIGNAL(VALUE1,V1_SIGNAL,VALUE2,V2_SLOT)               \
    connect(VALUE1,SIGNAL(V1_SIGNAL()),VALUE2 ,SLOT(V2_SLOT##Emittor()));
#define UNSYNCHRONIZE_SIGNAL(VALUE1,V1_SIGNAL,VALUE2,V2_SLOT)               \
    disconnect(VALUE1,SIGNAL(V1_SIGNAL()),VALUE2 ,SLOT(V2_SLOT##Emittor()));

//配列内に要素が存在しているかの確認
template<class C,class V>
bool exist(const C& array,const V& value){
    return std::find(std::begin(array),std::end(array),value) != std::end(array);
}
//配列内に要素が存在しているかの確認
template<class C,class V>
bool exist_if(const C& array,const V& func){
    return std::find_if(std::begin(array),std::end(array),func) != std::end(array);
}
//配列内の重複要素を削除
template<class C>
void unique(C& array){
    std::sort(std::begin(array),std::end(array));
    array.erase(std::unique(std::begin(array),std::end(array)),std::end(array));
}
//配列内の要素の番号を取得
template<class C,class V>
int IndexOf(const C& array,const V& value){
    typename C::const_iterator result = std::find(std::begin(array),std::end(array),value);
    if(result == std::end(array))return -1;
    else                         return std::distance(std::begin(array),result);
}

template<class C,class COMP>
int IndexOf_if(const C& array,COMP comp){
    typename C::const_iterator result = std::find_if(std::begin(array),std::end(array),comp);
    if(result == std::end(array))return -1;
    else                         return std::distance(std::begin(array),result);
}
//配列内から要素を全て削除
template<class C,class V>
void erase(C array,const V& value){
    auto it = std::find(std::begin(array),std::end(array),value);
    while(it != std::end(array)){
        array.erase(it);
        it = std::find(std::begin(array),std::end(array),value);
    }
}
//配列の型を変換
template<class In,class Out>
QList<Out> ConvertArrayType(QList<In> array){
    QList<Out> ans;
    for(In& tt:array){
        ans.push_back(dynamic_cast<Out>(tt));
    }
    return ans;
}

//剰余
template<class T,class U = T>
T Mod(T lhs,const U& rhs){
    return lhs - rhs * static_cast<int>(lhs/rhs);
}

//型比較
template<bool,class True_type,class FALSE_TYPE>
struct TypeIf{};
template<class True_type,class False_type>
struct TypeIf<true,True_type,False_type>{
    typedef True_type type;
};
template<class True_type,class False_type>
struct TypeIf<false,True_type,False_type>{
    typedef False_type type;
};

//Changeシグナル対象変数マクロ
#define OBSERVE_MEMBER(TYPE,CALL,NAME)        \
protected:                                    \
    TYPE NAME;                                \
public:                                       \
    TYPE get##CALL()const{return this->NAME;} \
    void set##CALL(TYPE v){                   \
        if(!(this->NAME == v)){               \
            this->NAME = v;                   \
            emit Changed(this);               \
        }                                     \
    }                                         \

//フラグ定義
#define DEFINE_FLAG(NAME,DEFAULT)                    \
protected:                                             \
    bool NAME = DEFAULT;                             \
public:                                              \
    virtual void Set##NAME(bool flag){NAME = flag;}  \
    virtual bool is##NAME ()const{return NAME;}      \

//ゲッターセッター定義
#define CREATEGETTER(TYPE,NAME,VALUE) \
TYPE get##NAME()const{                \
    return this->VALUE;               \
}
#define CREATESETTER(TYPE,NAME,VALUE) \
void set##NAME(const TYPE& arg){      \
    VALUE = arg;                      \
}
#define CREATEGETTER_ARRAY(TYPE,NAME,VALUE) \
TYPE get##NAME(int index)const{             \
    return this->VALUE[index];                  \
}
#define CREATESETTER_ARRAY(TYPE,NAME,VALUE) \
void set##NAME(const TYPE& arg,int index){      \
    VALUE[index] = arg;                      \
}
#define CREATE_IO(TYPE,NAME,VALUE)    \
CREATEGETTER(TYPE,NAME,VALUE)         \
CREATESETTER(TYPE,NAME,VALUE)

#define CREATE_IO(TYPE,NAME,VALUE)    \
CREATEGETTER(TYPE,NAME,VALUE)         \
CREATESETTER(TYPE,NAME,VALUE)

#endif // UTILS

