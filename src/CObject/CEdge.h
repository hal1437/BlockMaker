#ifndef CEDGE_H
#define CEDGE_H
#include "CObject.h"
#include "CPoint.h"

//エッジクラス
class CEdge :public CObject
{
    Q_OBJECT
protected:
    const static int COLLISION_SIZE = 5; //当たり判定半径

public:
    CPoint* start; //エッジの始点
    CPoint* end;   //エッジの終点

public:
    //作成関数(完了時:true , 継続時:false)
    virtual CREATE_RESULT Create(CPoint* pos) = 0;
    virtual bool Draw(QPainter& painter)const = 0;//描画関数
    virtual bool Move(const Pos& move) = 0;//移動関数
    virtual void Lock(bool lock) = 0;//ロック

    //中間点操作
    virtual int GetMiddleCount()const = 0;
    virtual CPoint* GetMiddle(int index)const = 0;
    virtual void    SetMiddle(CPoint*,int index) = 0;

    //近接点
    virtual Pos GetNear(const Pos& hand)const=0;
    virtual void SetStartPos(CPoint* pos);
    virtual void SetEndPos(CPoint* pos);

    //複製
    virtual CEdge* Clone()const = 0;

    CEdge(QObject* parent=nullptr);
    ~CEdge();

public slots:
    //点移動コールバック
    virtual void ChangePosCallback(const Pos& new_pos,const Pos& old_pos);

};

#endif // CEDGE_H
