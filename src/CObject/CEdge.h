#ifndef CEDGE_H
#define CEDGE_H
#include "CPoint.h"


//エッジクラス
class CEdge :public CObject
{
    Q_OBJECT

public:
    const static int COLLISION_SIZE   = 5; //当たり判定半径

public:
    CPoint* start; //エッジの始点
    CPoint* end;   //エッジの終点
    OBSERVE_MEMBER(double,Grading,grading)//エッジ寄せ
    OBSERVE_MEMBER(int   ,Divide ,divide)//分割数
public:
    //分割レート取得
    static double GetDivisionRate(int divide, double grading, int count);

public:
    virtual CREATE_RESULT Create(CPoint* pos) = 0;   //作成関数
    virtual void DrawArrow(double start,double end)const; //三次元描画関数
    virtual void DrawGL(Pos camera,Pos center)const; //三次元描画関数

    //中間点操作
    virtual CPoint*  GetPoint(int index) = 0;
    virtual CObject* GetChild(int index) = 0;
    virtual void     SetChild(int index,CObject* obj) = 0;
    virtual int      GetChildCount()const = 0;
    virtual Pos      GetMiddleDivide(double t)const = 0;//補間点取得
    virtual Pos      GetDivisionPoint(int count)const; //エッジ点取得
    virtual void     SetStartPos(CObject* obj);
    virtual void     SetEndPos(CObject* obj);

    //近接点
    virtual bool isOnEdge   (const Pos& hand)const;
    virtual Pos  GetNearPos (const Pos& hand)const;
    virtual Pos  GetNearLine(const Pos& pos1,const Pos& pos2)const;

    //複製
    virtual CObject* Clone()const = 0;

    //探索系
    Pos    GetDifferentialVec   (Pos pos)const; //posの位置で微分した基底ベクトル取得
    double GetMiddleParamFromPos(Pos pos)const; //tを求める

    CEdge(QObject* parent=nullptr);
    CEdge(CPoint* start,CPoint* end,QObject* parent=nullptr);
    ~CEdge();

public slots:
    //点移動コールバック
    virtual void ChangeChildCallback(QVector<CObject*> child);

};

#endif // CEDGE_H
