#ifndef CPOINT_H
#define CPOINT_H

#include "CObject.h"

//CAD上の点
class CPoint : public CObject,public Pos
{
private:
    const static int DRAWING_CIRCLE_SIZE = 5; //描画円半径
    const static int COLLISION_SIZE = 8;      //当たり判定半径

    bool control_point = false; //作用点

public:
    bool Create(CPoint* pos,int index); //作成関数

    virtual bool Draw(QPainter& painter)const ;//描画関数
    virtual bool Move(const Pos& diff);//移動関数
    //virtual void Lock(bool lock);//ロック

    //virtual bool isSelecting()   const;  //選択中
    //virtual bool isSelected()    const;  //選択済
    //virtual bool isCreating()    const;  //作成中
    //virtual bool isLock()        const;  //固定中
    virtual bool isSelectable()  const;  //選択可能
    virtual bool isControlPoint()const;  //作用点
    virtual bool ControlPoint(bool f);   //作用点設定

    //近接点
    virtual Pos GetNear(const Pos& hand)const;

    //ジョイント関係
    virtual int     GetJointNum()         const;
    virtual Pos     GetJointPos(int index)const;
    virtual CPoint* GetJoint   (int index);
    virtual void    SetJoint   (int, CPoint*);


    //コンストラクタ
    CPoint();
    CPoint(const Pos& pos);
    CPoint(double x,double y);//初期Create済みスターターセット
    ~CPoint();
};

#endif // CPOINT_H
