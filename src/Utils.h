#ifndef UTILS
#define UTILS

#include <iterator>
#include <algorithm>
#include <QStringList>
#include <tuple>
#define PI 3.1415926535897932384626433832795
#define TAB "    "
#define NEWLINE "\n"
#define MOUSE_ZOOM_RATE 10000.0

template<class T>
bool exist(std::iterator<std::forward_iterator_tag,T> begin ,std::iterator<std::forward_iterator_tag,T> end,const T& value){
    return std::find(begin,end,value);
}

template<class C,class V>
bool exist(const C& array,const V& value){
    return std::find(std::begin(array),std::end(array),value) != std::end(array);
}

template<class C,class V>
void erase(C array,const V& value){
    auto it = std::find(std::begin(array),std::end(array),value);
    while(it != std::end(array)){
        array.erase(it);
        it = std::find(std::begin(array),std::end(array),value);
    }
}

template<class T,class U = T>
T Mod(T lhs,const U& rhs){
    while(lhs > rhs)lhs -= rhs;
    return lhs;
}

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

