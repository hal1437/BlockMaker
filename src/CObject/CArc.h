#ifndef CARC_H
#define CARC_H
#include "CObject.h"
#include "CPoint.h"



class CArc : public CObject
{
private:
    const static int COLLISION_SIZE = 5; //当たり判定半径

protected:
    CPoint* pos[3]; //円弧の端点 0:中心
    double round = 0;//半径

public:

    //作成関数(完了時:true , 継続時:false)
    virtual bool Create(CPoint * pos,int index);

    //半径操作関数
    double GetRound()const;
    Pos    GetCenter()const;

    //更新関数
    virtual bool Refresh();

    virtual bool Draw(QPainter& painter)const;
    virtual bool Move(const Pos& diff);
    virtual void Lock(bool lock);

    //virtual bool isSelecting() const;  //選択中
    //virtual bool isSelected()  const;  //選択済
    //virtual bool isCreating()  const;  //作成中
    //virtual bool isLock()      const;  //固定中
    virtual bool isSelectable()const;  //mouse_posの位置で選択可能か

    //近接点
    virtual Pos  GetNear(const Pos& hand)const;

    //ジョイント関係
    virtual int     GetJointNum()         const;
    virtual Pos     GetJointPos(int index)const;
    virtual CPoint* GetJoint   (int index);
    virtual void    SetJoint   (int index,CPoint* ptr);


    CArc();
    ~CArc();
};


#endif // CARC_H
