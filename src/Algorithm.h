#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <iterator>
#include <cmath>
#include <algorithm>
#include <tuple>
#include <QStringList>
#include <QVector>
#include <QDebug>

//探索誤差
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

//最小値探索(三分探索)
template<class Func>
double MinimumSearch(double min,double max,Func function,double EPS = DEFAULT_SEARCH_EPS){
    while(max-min > EPS){
        double min_ = (max + min + min) / 3.0;
        double max_ = (max + max + min) / 3.0;
        if(function(max_) > function(min_))max = max_;
        else                               min = min_;
    }
    return (max+min)/2;
}


//前方差分
template<class Func>
double ForwardDifference(double param,Func function,double EPS = DEFAULT_SEARCH_EPS){
    return (function(param+EPS) - function(param)) /EPS;
}
//後方差分
template<class Func>
double BackwardDifference(double param,Func function,double EPS = DEFAULT_SEARCH_EPS){
    return (function(param) - function(param-EPS))/EPS;
}
//中央差分
template<class Func>
double CenterDifference(double param,Func function,double EPS = DEFAULT_SEARCH_EPS){
    return (function(param+EPS) - function(param-EPS))/(2*EPS);
}

//自動選択差分
template<class Func>
double AutoDifference(double min,double max,double param,Func function,double EPS = DEFAULT_SEARCH_EPS){
    if     (param + EPS > max)return BackwardDifference(max  ,function);//前方差分
    else if(param - EPS < min)return ForwardDifference (min  ,function);//後方差分
    else                      return CenterDifference  (param,function);//中央差分
}


#endif // ALGORITHM_H
