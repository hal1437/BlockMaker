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
    T z;
public:
    typedef Point<T> current;
    typedef const Point<T>& cr_current;
public:
    Point():x(0),y(0),z(0){}
    Point(const T& X,const T& Y):x(X),y(Y),z(0){}
    Point(const T& X,const T& Y,const T& Z):x(X),y(Y),z(Z){}
    Point(const QPoint& p):x(p.x()),y(p.y()),z(0){}

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
        return std::sqrt(x*x+y*y+z*z);
    }
    double Length(cr_current rhs)const{
        return std::sqrt(std::pow(x - rhs.x,2)+std::pow(y - rhs.y,2)+std::pow(z - rhs.z,2));
    }
    double Length2D()const{
        return std::sqrt(x*x+y*y);
    }
    double Length2D(cr_current rhs)const{
        return std::sqrt(std::pow(x - rhs.x,2)+std::pow(y - rhs.y,2));
    }
    double Dot(cr_current rhs)const{
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }
    double Dot2D(cr_current rhs)const{
        return x * rhs.x + y * rhs.y;
    }
    current GetNormalize()const{
        return current(x/Length(),y/Length(),z/Length());
    }
    current GetNormalize2D()const{
        return current(x/Length2D(),y/Length2D(),0);
    }
    current convert2D()const{
        return current(x,y,0);
    }

    current& Transform(QTransform rhs){
        QPointF p(x,y);
        p = p * rhs;
        this->x = p.x();
        this->y = p.y();
        return (*this);
    }


    current operator-()const{return Point(-x,-y,-z);}
    current operator+(cr_current rhs)const{return Point(x + rhs.x,y + rhs.y,z + rhs.z);}
    current operator-(cr_current rhs)const{return Point(x - rhs.x,y - rhs.y,z - rhs.z);}
    template<class V> current operator*(V rhs)const{return Point(x * rhs,y * rhs,z * rhs);}
    template<class V> current operator/(V rhs)const{return Point(x / rhs,y / rhs,z / rhs);}

    current& operator+=(cr_current rhs){x += rhs.x;y += rhs.y;z += rhs.z;return (*this);}
    current& operator-=(cr_current rhs){x -= rhs.x;y -= rhs.y;z -= rhs.z;return (*this);}
    template<class V> current operator*=(V rhs){x *= rhs;y *= rhs;z *= rhs;return (*this);}
    template<class V> current operator/=(V rhs){x /= rhs;y /= rhs;z /= rhs;return (*this);}

    bool operator==(cr_current rhs)const{
        return (this->x == rhs.x && this->y == rhs.y && this->z == rhs.z);
    }
    bool operator!=(cr_current rhs)const{
        return !(*this == rhs);
    }
    bool operator<(cr_current rhs)const{
        if(this->x == rhs.x){
            if(this->y == rhs.y){
                return (this->z < rhs.z);
            }else return (this->y < rhs.y);
        }else return (this->x < rhs.x);
    }
    operator QPointF(){
        return QPointF(this->x,this->y);
    }
};

template<class T>
std::ostream& operator<<(std::ostream& ost,Point<T> pos){
    ost << "(" << pos.x << "," << pos.y << "," << pos.z << ")";
    return ost;
}
template<class T>
std::istream& operator>>(std::istream& ost,Point<T>& pos){
    std::string str;
    QStringList list;
    ost >> str;
    list = QString(str.c_str()).replace('(',"").replace(')',"").split(',');
    pos.x = list[0].toDouble();
    pos.y = list[1].toDouble();
    if(list.size()>2){
        pos.z = list[2].toDouble();
    }else{
        pos.z = 0;
    }
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

