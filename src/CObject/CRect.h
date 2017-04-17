#ifndef CRECT_H
#define CRECT_H

#include "CObject.h"
#include "CPoint.h"
#include "CLine.h"
#include <memory>


//CAD上の長方形
class CRect : public CObject
{
private:
    const static int COLLISION_SIZE = 5;
protected:
    CPoint* pos[4];
    CLine*  lines[4];
    //12
    //34

    // 1
    //4 2
    // 3

public:

    //作成関数(完了時:true , 継続時:false)
    virtual bool Create(CPoint* pos,int index);

    virtual bool Draw(QPainter& painter)const;//描画関数
    virtual bool Move(const Pos& diff);//移動関数
    virtual void Lock(bool lock);//ロック

    //virtual bool isSelecting() const;  //選択中
    //virtual bool isSelected()  const;  //選択済
    //virtual bool isCreating()  const;  //作成中
    //virtual bool isLock()      const;  //固定中
    virtual bool isSelectable()const;  //mouse_posの位置で選択可能か

    //近接点
    virtual Pos GetNear(const Pos& hand)const;

    //線の取得
    CLine *GetLines(int index);

    //ジョイント関係
    virtual int     GetJointNum()         const;
    virtual Pos     GetJointPos(int index)const;
    virtual CPoint* GetJoint   (int index);
    virtual void    SetJoint   (int index,CPoint* ptr);

    CRect();
    ~CRect();
};


#endif // CRECT_H
