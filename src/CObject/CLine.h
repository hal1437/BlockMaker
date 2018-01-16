#ifndef CLINE_H
#define CLINE_H
#include "CEdge.h"

//直線
class CLine : public CEdge
{
    Q_OBJECT
public:
    virtual QString DefaultClassName(){return "CLine";}

    //作成関数(完了時:true , 継続時:false)
    virtual CREATE_RESULT Create(CPoint* pos);
    virtual void DrawGL(Pos camera,Pos center)const; //三次元描画関数
    virtual bool isSelectable(Pos pos)const;

    //中間点操作
    virtual CPoint*  GetPoint(int index);
    virtual CPoint*  GetPoint(int index)const;
    virtual CObject* GetChild(int index);
    virtual CObject* GetChild(int index)const;
    virtual void     SetChild(int index,CObject* obj);
    virtual int      GetChildCount()const;
    virtual Pos      GetMiddleDivide(double t)const;

    //近接点
    virtual Pos GetNearPos (const Pos& hand)const;
    virtual Pos GetNearLine(const Pos& pos1,const Pos& pos2)const;

    //複製
    virtual CEdge* Clone()const;

    //結合
    virtual CEdge*          MergeEdge (CEdge* merge); //失敗ならnullptr
     virtual QList<CEdge*> DivideEdge(CPoint* division);   //失敗ならQVector<CEdge*>()

    CLine(QObject* parent=nullptr);
    CLine(CPoint* start,CPoint* end,QObject* parent=nullptr);
    ~CLine();
};


#endif // CLINE_H
