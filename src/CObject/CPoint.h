#ifndef CPOINT_H
#define CPOINT_H

#include "CObject.h"

//三次元座標上の点
class CPoint : public CObject,public Pos
{
    Q_OBJECT
private:
    const static int DRAWING_CIRCLE_SIZE = 5; //描画円半径
public:
    static CPoint* hanged; //掴み点
    const static int COLLISION_SIZE = 10;      //当たり判定半径

    DEFINE_FLAG(ControlPoint,false) //制御点
    DEFINE_FLAG(Moving      ,false) //移動中

public:
    virtual QString DefaultClassName(){return "CPoint";}

    virtual CREATE_RESULT Create(CPoint* pos);       //作成関数
    virtual void DrawGL(Pos camera,Pos center)const; //描画関数
    virtual void MoveAbsolute(const Pos& diff);      //絶対移動関数
    virtual void MoveRelative(const Pos& diff);      //相対移動関数

    //子の操作
    virtual CObject* GetChild(int index);
    virtual void     SetChild(int index,CObject* obj);
    virtual int      GetChildCount()const;
    virtual QVector<CPoint*> GetAllChildren()const;

    //近接点
    virtual Pos GetNearPos (const Pos& hand)const;
    virtual Pos GetNearLine(const Pos& pos1,const Pos& pos2)const;

    //複製
    virtual CObject* Clone()const;

    //コンストラクタ
    CPoint(QObject* parent=nullptr);
    CPoint(const Pos& origin);
    CPoint(const Pos& pos,QObject* parent);
    ~CPoint();

    CPoint& operator=(const Pos& rhs);

};

#endif // CPOINT_H
