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

//シグナル発生スロット
#define DEFINE_EMITTOR(NAME)        \
void NAME##Emittor(){               \
    if(this->isPause() == false){   \
        emit NAME();                \
    }                               \
}
#define DEFINE_EMITTOR_ARG(NAME,ARG) \
void NAME##Emittor(){                \
    emit NAME(ARG);              \
}

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
//剰余
template<class T,class U = T>
T Mod(T lhs,const U& rhs){
    return lhs - rhs * static_cast<int>(lhs/rhs);
}

//Changeシグナル対象変数マクロ
#define OBSERVE_MEMBER(TYPE,CALL,NAME)        \
protected:                                    \
    TYPE NAME;                                \
public:                                       \
    TYPE get##CALL()const{return this->NAME;} \
    void set##CALL(TYPE v){                   \
        if(this->NAME != v){                  \
            this->NAME = v;                   \
            emit Changed(this);               \
        }                                     \
    }                                         \

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

#define CREATE_IO(TYPE,NAME,VALUE)    \
CREATEGETTER(TYPE,NAME,VALUE)         \
CREATESETTER(TYPE,NAME,VALUE)

//探索系
const double DEFAULT_SEARCH_EPS = 1.0e-7;

//二分探索
template<class Compare>
double BinarySearch(double min,double max,Compare comp,double EPS = DEFAULT_SEARCH_EPS){
    //bool comp(Value min ,Value max);
    //comp関数はmin < maxであることを比較する関数
    while(max-min > EPS){
        double half = (max+min)/2.0;
        if(comp(min,max))max = half;
        else             min = half;
    }
    return (max+min)/2.0;
}
//二変数二分探索
template<class Compare>
std::pair<double,double> FourBinarySearch(double min1,double max1,double min2,double max2,Compare comp,double EPS1 = DEFAULT_SEARCH_EPS,double EPS2 = DEFAULT_SEARCH_EPS){
    //std::pair<double,double> comp(double min1,double max1,double min2,double max2);
    //comp関数は4つの組み合わせの中でもっとも目的値に近い値を返す関数
    int count = 0;
    while(max1 - min1 > EPS1 || max2 - min2 > EPS2){
        double half1 = (min1 + max1) / 2.0;
        double half2 = (min2 + max2) / 2.0;
        std::pair<double,double> p = comp(min1,max1,min2,max2);
        if(p.first  == min1) max1 = half1;
        else                 min1 = half1;
        if(p.second == min2) max2 = half2;
        else                 min2 = half2;
        count++;
    }
    qDebug() << count;
    return std::make_pair((max1+min1)/2.0,(max2+min2)/2.0);
}

#endif // UTILS

