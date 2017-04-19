#ifndef CSPLINE_H
#define CSPLINE_H

#include "CEdge.h"
#include <memory>

#define MaxSplineSize 100
class Spline {
public:
    int num=0;
    double a[MaxSplineSize+1], b[MaxSplineSize+1], c[MaxSplineSize+1], d[MaxSplineSize+1];
public:
    Spline(){}
    void init(std::vector<double> sp);
    double culc(double t)const;
};

//CAD上の点
class CSpline : public CEdge
{
private:
    const static int DIVISION = 10;//分解数
    const static int DRAWING_LINE_SIZE = 3;//線の太さ
    const static int COLLISION_SIZE = 6;//あたり判定の大きさ
protected:
    std::vector<CPoint*> pos;
    Spline xs;
    Spline ys;

public:

    //作成関数(完了時:true , 継続時:false)
    virtual bool Create(CPoint* pos,int index);
    virtual bool Draw(QPainter& painter)const;//描画関数
    virtual bool Move(const Pos& move);//移動関数
    virtual void Lock(bool lock);//ロック

    //中間点操作
    virtual int GetMiddleCount()const;
    virtual CPoint* GetMiddle(int index);

    //近接点
    virtual Pos GetNear(const Pos& hand)const;

    CSpline(QObject* parent = nullptr);
    ~CSpline();
};

#endif // CSPLINE_H
