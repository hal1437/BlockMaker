#ifndef CPOINT_H
#define CPOINT_H

#include "CObject.h"

//CAD上の点
class CPoint : public CObject,public Pos
{
private:
    const static int DRAWING_CIRCLE_SIZE = 5; //描画半径
    const static int COLLISION_SIZE = 8;      //当たり判定半径

protected:
    bool selectable = true;    //選択可能かどうか
    bool control_point = false;//作用点

protected:
    bool Create(CPoint * pos,int index);//作成関数

public:
    virtual Pos 　GetNear(const Pos&)const;    //最近点
    virtual bool Draw(QPainter& painter)const; //描画関数
    virtual bool Move(const Pos& pos);         //移動関数
    virtual int GetJointNum()const;
    virtual Pos GetJointPos(int index)const;
    virtual CPoint* GetJoint(int index);

    virtual bool isSelectable()const;   //選択中
     virtual bool isControlPoint()const; //作用点
    virtual bool ControlPoint(bool f); //作用点か

    CPoint();
    CPoint(const Pos& pos);
    CPoint(double x,double y);
    ~CPoint();
};

#endif // CPOINT_H
