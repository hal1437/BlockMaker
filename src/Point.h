#ifndef POINT_H
#define POINT_H
#include <cmath>
#include <QPoint>
#include <QDebug>
#include <QTransform>
#include <initializer_list>
#include <type_traits>
#include <iostream>
#include <limits>
#include <tuple>
#include "Utils.h"

template<class T,std::size_t W,std::size_t H>
class Matrix{
public:
    T mat[W*H];
private:
    typedef Matrix<T,W,H> current;
    typedef const Matrix<T,W,H>& cr_current;
public:
    Matrix(){std::fill(mat,mat+W*H,0);}
    Matrix(cr_current origin){
        std::copy(origin.begin(),origin.end(),this->begin());
    }
    Matrix(const std::initializer_list<T> mat){
        std::copy(mat.begin(),mat.end(),this->begin());
    }
    Matrix(const T mat[W*H]     ){
        std::copy(mat,mat+W*H,this->begin());
    }

    //単位行列
    static current getIdentityMatrix(){
        static_assert(W==H,"This Matrix is not square matrix");
        current ans;
        for(std::size_t i=0;i<W;i++)ans.access(i,i) = 1;
        return ans;
    }
    static current getRotateXMatrix(double value){
        static_assert(W>3 && H>3,"This Matrix is not square matrix");
        current ans = getIdentityMatrix();
        ans.access(1,1) =  std::cos(value);
        ans.access(2,1) = -std::sin(value);
        ans.access(1,2) =  std::sin(value);
        ans.access(2,2) =  std::cos(value);
        return ans;
    }
    static current getRotateYMatrix(double value){
        static_assert(W>3 && H>3,"This Matrix is not square matrix");
        current ans = getIdentityMatrix();
        ans.access(0,0) =  std::cos(value);
        ans.access(2,0) =  std::sin(value);
        ans.access(0,2) = -std::sin(value);
        ans.access(2,2) =  std::cos(value);
        return ans;
    }
    static current getRotateZMatrix(double value){
        static_assert(W>3 && H>3,"This Matrix is not square matrix");
        current ans = getIdentityMatrix();
        ans.access(0,0) =  std::cos(value);
        ans.access(1,0) =  std::sin(value);
        ans.access(0,1) = -std::sin(value);
        ans.access(1,1) =  std::cos(value);
        return ans;
    }

public:

    T& access(int x,int y)     {return this->mat[y*W+x];}
    T  access(int x,int y)const{return this->mat[y*W+x];}

    //イテレータ
    T* begin()   {return this->mat;}
    T* end()     {return this->mat+H*W;}
    const T* begin()const{return this->mat;}
    const T* end()  const{return this->mat+H*W;}

    //演算子
    current operator-()const{return (*this)*-1;}
    current operator+(current rhs)const{
        Matrix ans;
        for(std::size_t i=0;i<H;i++)for(std::size_t j=0;j<W;j++){
            ans.mat[i*W+j] = this->mat[i*W+j] + rhs.mat[i*W+j];
        }
        return ans;
    }
    current operator-(current rhs)const{
        Matrix ans;
        for(std::size_t i=0;i<H;i++)for(std::size_t j=0;j<W;j++){
            ans.mat[i*W+j] = this->mat[i*W+j] - rhs.mat[i*W+j];
        }
        return ans;
    }
    template<class V> current operator*(V rhs)const{
        Matrix ans;
        for(std::size_t i=0;i<H;i++)for(std::size_t j=0;j<W;j++){
            ans.mat[i*W+j] = this->mat[i*W+j] * rhs;
        }
        return ans;
    }
    template<class V> current operator/(V rhs)const{
        Matrix ans;
        for(std::size_t i=0;i<H;i++)for(std::size_t j=0;j<W;j++){
            ans.mat[i*W+j] = this->mat[i*W+j] / rhs;
        }
        return ans;
    }

    //内積
    template<std::size_t N>
    Matrix<T,N,H> Dot(Matrix<T,N,W> rhs){// W*H * N*W
        Matrix<T,N,H> ans;
        for(std::size_t i=0;i<H;i++){
            for(std::size_t j=0;j<N;j++){
                double sum = 0;
                for(std::size_t piv=0;piv<W;piv++){
                    sum += this->mat[i * W + piv] * rhs.mat[piv * W + j];
                }
                ans.mat[i*N+j] = sum;
            }
        }
        return ans;
    }
    //外積
    current Cross(Matrix<T,W,H> rhs){// W*H * N*W
        static_assert(H==1,"This Matrix height is not 1.");
        current ans;
        ans.mat[0] = this->mat[1] * rhs.mat[2] - this->mat[2] * rhs.mat[1];
        ans.mat[1] = this->mat[2] * rhs.mat[0] - this->mat[0] * rhs.mat[2];
        ans.mat[2] = this->mat[0] * rhs.mat[1] - this->mat[1] * rhs.mat[0];
        return ans;
    }



    //代入演算子
                      current& operator+=(current rhs){*this = *this + rhs;return (*this);}
                      current& operator-=(current rhs){*this = *this - rhs;return (*this);}
    template<class V> current& operator*=(V rhs)      {*this = *this * rhs;return (*this);}
    template<class V> current& operator/=(V rhs)      {*this = *this / rhs;return (*this);}

    //比較演算子
    bool operator==(cr_current rhs)const{
        return std::equal(this->begin(),this->end(),rhs.begin());
    }
    bool operator!=(cr_current rhs)const{return !(*this == rhs);}
    bool operator<(cr_current rhs)const{
        for(std::size_t i=0;i<H;i++){
            for(std::size_t j=0;j<W;j++){
                if(this->access(i,j) != rhs.access(i,j))return(this->access(i,j) < rhs.access(i,j));
            }
        }
        return false;
    }

};
typedef Matrix<double,3,3> Mat;
typedef Matrix<double,4,4> Quat;



//座標
template<class T>
struct Point : public Matrix<T,4,1>{
//    T x;
//    T y;
//    T z;
//    T 1;
private:
    typedef Matrix<T,4,1> base;
    typedef Point<T> current;
    typedef const Point<T>& cr_current;
public:
    Point():base({0,0,0,1}){}
    Point(const T& X,const T& Y,const T& Z = 0){
        this->mat[0] = X;
        this->mat[1] = Y;
        this->mat[2] = Z;
        this->mat[3] = 1;}
    Point(const QPoint& p):Point(p.x(),p.y(),0,1){}
    Point(const base& p):base(p){}

    T& x(){return this->mat[0];}
    T& y(){return this->mat[1];}
    T& z(){return this->mat[2];}
    T x()const{return this->mat[0];}
    T y()const{return this->mat[1];}
    T z()const{return this->mat[2];}
    T w()const{return this->mat[3];}

    current operator-()                       const{current ans = (-(*static_cast<const base*>(this)));                       ans.mat[3] = 1; return ans;}
    current operator+(current rhs)            const{current ans = (*static_cast<const base*>(this) + static_cast<base>(rhs)); ans.mat[3] = 1; return ans;}
    current operator-(current rhs)            const{current ans = (*static_cast<const base*>(this) - static_cast<base>(rhs)); ans.mat[3] = 1; return ans;}
    template<class V> current operator*(V rhs)const{current ans = (*static_cast<const base*>(this) * rhs);                    ans.mat[3] = 1; return ans;}
    template<class V> current operator/(V rhs)const{current ans = (*static_cast<const base*>(this) / rhs);                    ans.mat[3] = 1; return ans;}

    //直線と点の最近点を求める
    static current LineNearPoint(cr_current pos1,cr_current pos2,cr_current hand){
        //内積で一発
        return current(pos1 + current(pos2-pos1).GetNormalize() * current(pos2-pos1).GetNormalize().DotPos(hand-pos1));
    }

    //円と点の最近点を求める
    static current CircleNearPoint(cr_current center,double r,cr_current hand){
        //当然centerとhandの線分上にある
        return current(hand-center).GetNormalize() * r + center;
    }
    //方向比較
    static bool DirComp(cr_current lhs,cr_current rhs){
        return (lhs.GetNormalize() == rhs.GetNormalize() || lhs.GetNormalize() == -rhs.GetNormalize());
    }
    //角度比較
    static double Angle2D(cr_current base,cr_current dir){
        current a = base.GetNormalize();
        current b = dir.GetNormalize();
        if(a.DotPos(b) == 0){
            return !MoreThan(current(),b,a)*180+90;
        }
        if(MoreThan(current(),a,b)){
            if(a.x() > 0)return 360 - std::acos(a.x()*b.x() + a.y()*b.y()) * 180 / PI;
            else return std::acos(a.x()*b.x() + a.y()*b.y()) * 180 / PI;
        }else {
            if(a.x() < 0)return 360 - std::acos(a.x()*b.x() + a.y()*b.y()) * 180 / PI;
            else return std::acos(a.x()*b.x() + a.y()*b.y()) * 180 / PI;
        }
    }
    //角度比較
    static double Angle(cr_current base,cr_current dir){
        current a = base.GetNormalize();
        current b = dir.GetNormalize();
        if(a.DotPos(b) == 0){
            return 90;
        }
        return std::acos(a.DotPos(b)) * 180 / PI;
    }

    //任意軸回転
    static current RodriguesRotate(current value,current base,double theta){
        current n = base.GetNormalize();
        current r = value;//.GetNormalize();
        return n * (n.DotPos(r)) + (r - n*(n.DotPos(r)))*std::cos(theta) - (r.Cross(n)*std::sin(theta));
    }


    //直線以上
    static bool MoreThan(cr_current pos1,cr_current pos2,cr_current hand){
        //y=ax+b
        //b=y-ax
        const T a = (pos2.y() - pos1.y())/(pos2.x() - pos1.x());
        const T b = pos1.y() - a*pos1.x();
        return ((hand.x() * a + b) <= hand.y());
    }

    double Length()const{
        return std::sqrt(x()*x()+y()*y()+z()*z());
    }
    double Length(cr_current rhs)const{
        return std::sqrt(std::pow(x() - rhs.x(),2)+
                         std::pow(y() - rhs.y(),2)+
                         std::pow(z() - rhs.z(),2));
    }
    double Length2D()const{
        return std::sqrt(x()*x()+y()*y());
    }
    double Length2D(cr_current rhs)const{
        return std::sqrt(std::pow(x() - rhs.x(),2)+
                         std::pow(y() - rhs.y(),2));
    }
    double DotPos(cr_current rhs)const{
        return x() * rhs.x() + y() * rhs.y() + z() * rhs.z();
    }
    double DotPos2D(cr_current rhs)const{
        return x() * rhs.x() + y() * rhs.y();
    }
    current GetNormalize()const{
        return current(x()/Length(),y()/Length(),z()/Length());
    }
    current GetNormalize2D()const{
        return current(x()/Length2D(),y()/Length2D(),0);
    }
    current convert2D()const{
        return current(x(),y(),0);
    }

    current& Transform(QTransform rhs){
        QPointF p(x(),y());
        p = p * rhs;
        this->x() = p.x();
        this->y() = p.y();
        return (*this);
    }

    operator QPointF(){
        return QPointF(this->x(),this->y());
    }
};

template<class T>
std::ostream& operator<<(std::ostream& ost,Point<T> pos){
    ost << "(" << pos.x() << "," << pos.y() << "," << pos.z() << ")";
    return ost;
}
template<class T>
std::istream& operator>>(std::istream& ost,Point<T>& pos){
    std::string str;
    QStringList list;
    ost >> str;
    list = QString(str.c_str()).replace('(',"").replace(')',"").split(',');
    pos.x() = list[0].toDouble();
    pos.y() = list[1].toDouble();
    if(list.size()>2){
        pos.z() = list[2].toDouble();
    }else{
        pos.z() = 0;
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

//角方向
enum class CORNER_LIM{
    MIN,
    MAX,
};

#endif // POINT_H

