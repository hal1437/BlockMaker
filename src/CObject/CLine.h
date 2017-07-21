#ifndef CLINE_H
#define CLINE_H
#include "CEdge.h"

//CAD上の点
class CLine : public CEdge
{
    Q_OBJECT
public:
    //作成関数(完了時:true , 継続時:false)
    virtual CREATE_RESULT Create(CPoint* pos);
    virtual bool Move(const Pos& move);//移動関数
    virtual void SetLock(bool lock);//ロック

    virtual bool isSelectable(Pos pos)const;//選択可能関数(再定義)

    //中間点操作
    virtual int     GetMiddleCount()const;
    virtual CPoint* GetMiddle(int index)const;
    virtual void    SetMiddle(CPoint*,int index);
    virtual Pos     GetMiddleDivide(double t)const;    //補完点

    //近接点
    virtual Pos GetNearPos (const Pos& hand)const;
    //virtual Pos GetNearLine(const Pos& pos1,const Pos& pos2)const;

    //複製
    virtual CEdge* Clone()const;

    CLine(QObject* parent=nullptr);
    CLine(CPoint* start,CPoint* end,QObject* parent=nullptr);
    ~CLine();
};


#endif // CLINE_H
