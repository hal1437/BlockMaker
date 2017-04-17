#ifndef CLINE_H
#define CLINE_H
#include "CObject.h"
#include "CPoint.h"
#include <memory>

//CAD上の点
class CLine : public CObject
{
private:
    const static int COLLISION_SIZE = 5; //当たり判定半径

protected:
    CPoint* pos[2]; //線の端の点

public:

    //作成関数(完了時:true , 継続時:false)
    virtual bool Create(CPoint* pos,int index);

    virtual bool Draw(QPainter& painter)const;//描画関数
    virtual bool Move(const Pos& move);//移動関数
    virtual void Lock(bool lock);//ロック

    //virtual bool isSelecting() const;  //選択中
    //virtual bool isSelected()  const;  //選択済
    //virtual bool isCreating()  const;  //作成中
    //virtual bool isLock()      const;  //固定中
    virtual bool isSelectable()const;  //mouse_posの位置で選択可能か

    //近接点
    virtual Pos GetNear(const Pos& hand)const;

    //ジョイント関係
    virtual int     GetJointNum()         const;
    virtual Pos     GetJointPos(int index)const;
    virtual CPoint* GetJoint   (int index);
    virtual void    SetJoint   (int index, CPoint* ptr);

    //距離
    double DistanceToPoint(const Pos& pos)const;

    CLine();
    ~CLine();
};


#endif // CLINE_H
