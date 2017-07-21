#ifndef CEDGE_H
#define CEDGE_H
#include "CObject.h"
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

public:
    //作成関数(完了時:true , 継続時:false)
    virtual CREATE_RESULT Create(CPoint* pos) = 0;
    virtual bool DrawGL(Pos camera,Pos center)const;//三次元描画関数
    virtual bool Move(const Pos& move) = 0;//移動関数

    //中間点操作
    virtual int     GetMiddleCount()const = 0;
    virtual CPoint* GetMiddle(int index)const = 0;
    virtual void    SetMiddle(CPoint*,int index) = 0;
    virtual Pos     GetMiddleDivide(double t)const = 0;    //補完点

    //全ノード取得
    virtual QVector<CPoint*> GetAllPoints()const;//全ての構成点を取得
    virtual int     GetPointSequenceCount()const;
    virtual CPoint* GetPointSequence(int index)const;
    virtual void    SetPointSequence(CPoint* pos,int index);

    //近接点
    virtual Pos GetNearPos (const Pos& hand)const = 0;
    virtual Pos GetNearLine(const Pos& pos1,const Pos& pos2)const;

    //端点操作
    virtual void SetStartPos(CPoint* pos);
    virtual void SetEndPos(CPoint* pos);

    //複製
    virtual CObject* Clone()const = 0;

    CEdge(QObject* parent=nullptr);
    CEdge(CPoint* start,CPoint* end,QObject* parent=nullptr);
    ~CEdge();

public slots:
    //点移動コールバック
    virtual void ChangePosCallback(CPoint* pos,Pos old_pos);

};

#endif // CEDGE_H
