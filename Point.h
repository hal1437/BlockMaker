#ifndef POINT_H
#define POINT_H
#include <cmath>
#include <QPoint>
#include <iostream>


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

    //直線と点の最近点を求める
    static current LineNearPoint(cr_current pos1,cr_current pos2,cr_current hand){
        //内積で一発
        return (pos1 + (pos2-pos1).GetNormalize() * (pos2-pos1).GetNormalize().Dot(hand-pos1));
    }

    //円と点の最近点を求める
    static current CircleNearPoint(cr_current center,double r,cr_current hand){
        //当然centerとhandの線分上にある
        return (hand-center).GetNormalize() * r + center;
    }
    //方向比較
    static bool DirComp(cr_current lhs,cr_current rhs){
        return (lhs.GetNormalize() == rhs.GetNormalize() || lhs.GetNormalize() == -rhs.GetNormalize());
    }

    double Length()const{
        return std::sqrt(x*x+y*y);
    }
    double Length(cr_current rhs)const{
        return std::sqrt(std::pow(x - rhs.x,2)+std::pow(y - rhs.y,2));
    }
    double Dot(cr_current rhs)const{
        return x * rhs.x + y * rhs.y;
    }
    current GetNormalize()const{
        return current(x/Length(),y/Length());
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

    bool operator==(cr_current rhs)const{
        return (this->x == rhs.x && this->y == rhs.y);
    }
    bool operator!=(cr_current rhs)const{
        return !(*this == rhs.x);
    }
    bool operator<(cr_current rhs)const{
        if(this->x == rhs.x){
            return (this->y == rhs.y);
        }else return (this->x == rhs.x);
    }
};

template<class T>
std::ostream& operator<<(std::ostream& ost,Point<T> pos){
    ost << "(" << pos.x << "," << pos.y << ")";
    return ost;
}

typedef Point<double> Pos;

#endif // POINT_H

