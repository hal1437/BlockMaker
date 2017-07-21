#ifndef CARC_H
#define CARC_H
#include "CEdge.h"

class CArc : public CEdge
{
    Q_OBJECT
protected:
    double round = 0;//半径

public:
    CPoint* center; //中心
    bool reverse = false;
public:
    //半径操作関数
    double GetRound()const;

public:
    void SetReverse(bool reverse){this->reverse = reverse;}
    bool GetReverse(){return this->reverse;}

    //作成関数(完了時:true , 継続時:false)
    virtual CREATE_RESULT Create(CPoint* pos);
    virtual bool DrawGL(Pos camera,Pos center)const;
    virtual bool Move(const Pos& diff);
    virtual void SetLock(bool lock);
    virtual bool isSelectable(Pos pos) const;//選択可能オーバーライド

    //始点終点操作オーバーライド
    virtual void SetStartPos (CPoint* pos);
    virtual void SetEndPos   (CPoint* pos);
    virtual void SetCenterPos(CPoint* pos);

    //中間点操作
    virtual int GetMiddleCount()const;
    virtual CPoint* GetMiddle(int index)const;
    virtual void    SetMiddle(CPoint*,int index);
    virtual Pos     GetMiddleDivide(double t)const;

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
    virtual void ChangePosCallback(CPoint* pos,Pos old_pos);

};


#endif // CARC_H
