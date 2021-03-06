#ifndef CARC_H
#define CARC_H
#include "CEdge.h"

class CArc : public CEdge
{
    Q_OBJECT
protected:
    double round_s = 0;//半径
    double round_e = 0;//半径
    const static int LINE_NEAR_DIVIDE = 30; //近似直線判定関数の分割数

public:
    CPoint* center; //中心
    DEFINE_FLAG(Reverse,false)

public:
    //半径操作関数
    //double GetRound()const;

    virtual QString DefaultClassName(){return "CArc";}

    //作成関数(完了時:true , 継続時:false)
    virtual CREATE_RESULT Create(CPoint* pos);
    virtual void DrawGL(Pos camera,Pos center)const;
    virtual bool isSelectable(Pos pos) const;//選択可能オーバーライド

    //中間点操作
    virtual CPoint*  GetPoint(int index);
    virtual CPoint*  GetPoint(int index)const;
    virtual CObject* GetChild(int index);
    virtual CObject* GetChild(int index)const;
    virtual void     SetChild(int index,CObject* obj);
    virtual int      GetChildCount()const;
    virtual Pos      GetMiddleDivide(double t)const;    //補完点
    virtual void     SetCenterPos(CPoint* pos);

    //近接点
    virtual Pos GetNearPos (const Pos& hand)const;
    //virtual Pos GetNearLine(const Pos& pos1,const Pos& pos2)const;  //省略

    //複製
    virtual CEdge* Clone()const;

    CArc(QObject* parent=nullptr);
    CArc(CPoint* start,CPoint* end,QObject* parent = nullptr);
    ~CArc();

public slots:
    //点移動コールバックオーバーライド
    virtual void ChangeChildCallback(QList<CObject*> child);

};


#endif // CARC_H
