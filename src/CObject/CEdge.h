#ifndef CEDGE_H
#define CEDGE_H
#include "CPoint.h"

//エッジクラス
class CEdge :public CObject
{
    Q_OBJECT

public:
    const static int COLLISION_SIZE   = 5; //当たり判定半径
    const static int LINE_NEAR_DIVIDE = 100; //近似直線判定関数の分割数

public:
    CPoint* start; //エッジの始点
    CPoint* end;   //エッジの終点
    double grading = 1.0; //エッジ寄せ
    int    divide  = 0; //分割数(0:自動的に決定)

public:
    virtual CREATE_RESULT Create(CPoint* pos) = 0;   //作成関数
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
    virtual Pos GetNearPos (const Pos& hand)const = 0;
    virtual Pos GetNearLine(const Pos& pos1,const Pos& pos2)const;

    //複製
    virtual CObject* Clone()const = 0;

    CEdge(QObject* parent=nullptr);
    ~CEdge();

public slots:
    //点移動コールバック
    virtual void ChangeChildCallback(CObject* child);

};

#endif // CEDGE_H
