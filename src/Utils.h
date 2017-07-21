#ifndef UTILS
#define UTILS

#include <iterator>
#include <algorithm>
#include <QStringList>
#include <QVector>
#include <tuple>
#define PI M_PI
#define TAB "    "
#define NEWLINE "\n"
#define MOUSE_ZOOM_RATE 10000.0
#define NearlyEqual(RHS,LHS) (std::abs(RHS-LHS) < 0.000001)


//監視オブジェクトの定義、関数宣言を行う
#define DEFINE_OBSERVER(TYPE,NAME)               \
private:                                         \
    QVector<TYPE> NAME;                          \
public slots:                                    \
    QVector<TYPE>& Get##NAME(){return NAME;}     \
    QVector<TYPE>  Get##NAME()const{return NAME;}\
    inline void Add##NAME(TYPE value){           \
        NAME.push_back(value);                   \
        emit Update##NAME();                     \
    }                                            \
    inline void Remove##NAME(TYPE value){        \
        NAME.removeAll(value);                   \
        emit Update##NAME();                     \
    }                                            \


//配列内に要素が存在しているかの確認
template<class C,class V>
bool exist(const C& array,const V& value){
    return std::find(std::begin(array),std::end(array),value) != std::end(array);
}
//配列内の重複要素を削除
template<class C>
bool unique(C& array){
    std::sort(std::begin(array),std::end(array));
    array.erase(std::unique(std::begin(array),std::end(array)),std::end(array));
}
//配列内の要素の番号を取得
template<class C,class V>
int IndexOf(const C& array,const V& value){
    typename C::const_iterator result = std::find(std::begin(array),std::end(array),value);
    if(result == std::end(array))return -1;
    else                           return std::distance(std::begin(array),result);
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
//剰余
template<class T,class U = T>
T Mod(T lhs,const U& rhs){
    return lhs - rhs * static_cast<int>(lhs/rhs);
}

//フラグ定義
#define DEFINE_FLAG(NAME,DEFAULT)                    \
private:                                             \
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

#define CREATE_IO_ARRAY(TYPE,NAME,VALUE)    \
CREATEGETTER_ARRAY(TYPE,NAME,VALUE)         \
CREATESETTER_ARRAY(TYPE,NAME,VALUE)









#endif // UTILS

