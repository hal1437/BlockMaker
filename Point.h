#ifndef POINT_H
#define POINT_H
#include <cmath>

//座標
template<class T>
struct Point{
    T x;
    T y;
public:
    typedef Point<T> current;
    typedef const Point<T>& cr_current;
public:
    Point():x(0),y(0){}
    Point(const T& X,const T& Y):x(X),y(Y){}

    double Length(cr_current rhs)const{
        return std::sqrt(std::pow(x - rhs.x,2)+std::pow(y - rhs.y,2));
    }

    current operator-()const{return Point(-x,-y);}
    current operator+(cr_current rhs)const{return Point(x + rhs.x,y + rhs.y);}
    current operator-(cr_current rhs)const{return Point(x - rhs.x,y - rhs.y);}
    template<class V> current operator*(V rhs)const{return Point(x * rhs,y * rhs);}
    template<class V> current operator/(V rhs)const{return Point(x / rhs,y / rhs);}

    current& operator+=(cr_current rhs){x += rhs.x;y += rhs.y;return (*this);}
    current& operator-=(cr_current rhs){x -= rhs.x;y -= rhs.y;return (*this);}
    template<class V> current operator*(V rhs){x *= rhs;y *= rhs;return (*this);}
    template<class V> current operator/(V rhs){x /= rhs;y /= rhs;return (*this);}
};

typedef Point<double> Pos;

#endif // POINT_H

