#ifndef CARC_H
#define CARC_H
#include "CEdge.h"



class CArc : public CEdge
{
protected:
    double round = 0;//半径

public:
    CPoint* center; //中心

public:
    //半径操作関数
    double GetRound()const;

public:

    //作成関数(完了時:true , 継続時:false)
    virtual CREATE_RESULT Create(CPoint* pos);
    virtual bool Draw(QPainter& painter)const;
    virtual bool Move(const Pos& diff);
    virtual void Lock(bool lock);
    virtual bool isSelectable(Pos pos) const;//選択可能オーバーライド

    //始点終点操作オーバーライド
    virtual void SetStartPos(CPoint* pos);
    virtual void SetEndPos(CPoint* pos);

    //中間点操作
    virtual int GetMiddleCount()const;
    virtual CPoint* GetMiddle(int index)const;
    virtual void    SetMiddle(CPoint*,int index);

    //近接点
    virtual Pos GetNear(const Pos& hand)const;
    //複製
    virtual CEdge* Clone()const;

    CArc(QObject* parent=nullptr);
    ~CArc();

public slots:
    //点移動コールバックオーバーライド
    virtual void ChangePosCallback(const Pos& new_pos,const Pos& old_pos);

};


#endif // CARC_H
