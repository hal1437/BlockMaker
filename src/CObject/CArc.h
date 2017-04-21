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
    virtual CREATE_RESULT Create(CPoint* start);
    virtual bool Draw(QPainter& painter)const;
    virtual bool Move(const Pos& diff);
    virtual void Lock(bool lock);

    //中間点操作
    virtual int GetMiddleCount()const;
    virtual CPoint* GetMiddle(int index);

    //近接点
    virtual Pos GetNear(const Pos& hand)const;

    CArc(QObject* parent=nullptr);
    ~CArc();

public slots:
    //点移動コールバックオーバーライド
    virtual void ChangePosCallback(const Pos& new_pos,const Pos& old_pos);

};


#endif // CARC_H
