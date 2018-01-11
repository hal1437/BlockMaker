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
    Q_OBJECT
private:
    const static int DIVISION = 100;//分解数
    const static int LINE_NEAR_DIVIDE = 50; //近似直線判定関数の分割数
protected:
    std::vector<CPoint*> pos;
    Spline xs;
    Spline ys;
    Spline zs;

public:
    static QString DefaultClassName(){return "CSpline";}

    //作成関数(完了時:true , 継続時:false)
    virtual CREATE_RESULT Create(CPoint* pos);
    virtual void DrawGL(Pos camera,Pos center)const;

    //中間点操作
    virtual CPoint*  GetPoint(int index);
    virtual CObject* GetChild(int index);
    virtual void     SetChild(int index,CObject* obj);
    virtual int      GetChildCount()const;
    virtual Pos      GetMiddleDivide(double t)const;    //補完点

    //近接点
    virtual Pos GetNearPos (const Pos& hand)const;
    virtual Pos GetNearLine(const Pos& pos1,const Pos& pos2)const;

    //複製
    CEdge* Clone()const;

    //更新
    virtual void Refresh();

    CSpline(QObject* parent = nullptr);
    CSpline(CPoint* start,CPoint* end,QObject* parent = nullptr);
    ~CSpline();

public slots:
    //点移動コールバックオーバーライド
    virtual void ChangeChildCallback(QVector<CObject*>);

};

#endif // CSPLINE_H
