#ifndef UTILS
#define UTILS

#include <iterator>
#include <algorithm>
#include <QStringList>
#define PI 3.1415926535897932384626433832795

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


#endif // UTILS

