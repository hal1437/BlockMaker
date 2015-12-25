#ifndef CPOINT_H
#define CPOINT_H

#include "CObject.h"

//CAD上の点
class CPoint : public CObject,public Relative<Pos>
{
private:
    const static int DRAWING_CIRCLE_SIZE = 5;
    const static int COLLISION_SIZE = 8;

protected:
    bool Create(Relative<Pos> pos,int index);

public:
    virtual Pos GetNear(const Pos&)const;
    virtual bool Draw(QPainter& painter)const;
    virtual bool Selecting();
    virtual bool isLocked();
    virtual bool Move(const Pos& diff);
    virtual int GetJointNum()const;
    virtual Pos GetJointPos(int index)const;


    CPoint();
    ~CPoint();
};

#endif // CPOINT_H
