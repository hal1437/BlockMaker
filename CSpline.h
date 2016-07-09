#ifndef CSPLINE_H
#define CSPLINE_H

#include "CObject.h"
#include "CPoint.h"
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
class CSpline : public CObject
{
private:
    const static int DIVISION = 10;//分解数
    const static int DRAWING_LINE_SIZE = 3;//線の太さ
    const static int COLLISION_SIZE = 6;//あたり判定の大きさ
protected:
    std::vector<CPoint*> pos;
    Spline xs;
    Spline ys;

    virtual bool Create(CPoint* pos, int index);
public:

    virtual bool Refresh();
    virtual Pos GetNear(const Pos& hand)const;
    virtual bool Draw(QPainter& painter)const;
    virtual bool Selecting();
    virtual void Lock(bool lock);
    virtual bool Move(const Pos& diff);
    virtual int GetJointNum()const;
    virtual Pos GetJointPos(int index)const;
    virtual CPoint* GetJoint(int index);



    CSpline();
    ~CSpline();
};

#endif // CSPLINE_H
