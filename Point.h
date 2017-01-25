#ifndef POINT_H
#define POINT_H
#include <cmath>
#include <QPoint>
#include <QTransform>
#include <iostream>
#include <limits>
#include "Utils.h"


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
    Point(const QPoint& p):x(p.x()),y(p.y()){}

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
    //角度比較
    static double Angle(cr_current base,cr_current dir){
        current a = base.GetNormalize();
        current b = dir.GetNormalize();
        if(a.Dot(b) == 0){
            return !MoreThan(current(),b,a)*180+90;
        }
        if(MoreThan(current(),a,b)){
            if(a.x > 0)return 360 - std::acos(a.x*b.x + a.y*b.y) * 180 / PI;
            else return std::acos(a.x*b.x + a.y*b.y) * 180 / PI;
        }
        else {
            if(a.x < 0)return 360 - std::acos(a.x*b.x + a.y*b.y) * 180 / PI;
            else return std::acos(a.x*b.x + a.y*b.y) * 180 / PI;
        }
    }

    //直線以上
    static bool MoreThan(cr_current pos1,cr_current pos2,cr_current hand){
        //y=ax+b
        //b=y-ax
        const T a = (pos2.y - pos1.y)/(pos2.x - pos1.x);
        const T b = pos1.y - a*pos1.x;
        return ((hand.x * a + b) <= hand.y);
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
    current& Transform(QTransform rhs){
        QPointF p(x,y);
        p = p * rhs;
        this->x = p.x();
        this->y = p.y();
        return (*this);
    }


    current operator-()const{return Point(-x,-y);}
    current operator+(cr_current rhs)const{return Point(x + rhs.x,y + rhs.y);}
    current operator-(cr_current rhs)const{return Point(x - rhs.x,y - rhs.y);}
    template<class V> current operator*(V rhs)const{return Point(x * rhs,y * rhs);}
    template<class V> current operator/(V rhs)const{return Point(x / rhs,y / rhs);}

    current& operator+=(cr_current rhs){x += rhs.x;y += rhs.y;return (*this);}
    current& operator-=(cr_current rhs){x -= rhs.x;y -= rhs.y;return (*this);}
    template<class V> current operator*=(V rhs){x *= rhs;y *= rhs;return (*this);}
    template<class V> current operator/=(V rhs){x /= rhs;y /= rhs;return (*this);}

    bool operator==(cr_current rhs)const{
        return (this->x == rhs.x && this->y == rhs.y);
    }
    bool operator!=(cr_current rhs)const{
        return !(*this == rhs);
    }
    bool operator<(cr_current rhs)const{
        if(this->x == rhs.x){
            return (this->y < rhs.y);
        }else return (this->x < rhs.x);
    }
};

template<class T>
std::ostream& operator<<(std::ostream& ost,Point<T> pos){
    ost << "(" << pos.x << "," << pos.y << ")";
    return ost;
}

namespace std {
    template<class T>
    class numeric_limits<Point<T>> {
    public:

        static Point<T> max(){
            return Point<T>(std::numeric_limits<T>::max(),std::numeric_limits<T>::max());
        }
        static Point<T> lowest(){
            return Point<T>(std::numeric_limits<T>::lowest(),std::numeric_limits<T>::lowest());
        }
    };
}


typedef Point<double> Pos;

#endif // POINT_H

