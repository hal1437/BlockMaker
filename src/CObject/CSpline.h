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
    const static int DIVISION = 10;//分解数
    const static int DRAWING_LINE_SIZE = 3;//線の太さ
    const static int COLLISION_SIZE = 6;//あたり判定の大きさ
protected:
    std::vector<CPoint*> pos;
    Spline xs;
    Spline ys;
    Spline zs;

public:

    //作成関数(完了時:true , 継続時:false)
    virtual CREATE_RESULT Create(CPoint* pos);
    virtual bool Move(const Pos& move);//移動関数
    virtual void SetLock(bool lock);//ロック

    //始点終点操作オーバーライド
    virtual void SetStartPos(CPoint* pos);
    virtual void SetEndPos(CPoint* pos);

    //中間点操作
    virtual int GetMiddleCount()const;
    virtual CPoint* GetMiddle(int index)const;
    virtual void    SetMiddle(CPoint*,int index);
    virtual Pos     GetMiddleDivide(double t)const;    //補完点


    //近接点
    virtual Pos GetNearPos (const Pos& hand)const;
    //virtual Pos GetNearLine(const Pos& pos1,const Pos& pos2)const;

    //複製
    CEdge* Clone()const;


    void RefreshNodes();

    CSpline(QObject* parent = nullptr);
    CSpline(CPoint* start,CPoint* end,QObject* parent = nullptr);
    ~CSpline();

public slots:
    //点移動コールバックオーバーライド
    virtual void ChangePosCallback(CPoint* new_pos,Pos old_pos);

};

#endif // CSPLINE_H
