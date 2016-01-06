#ifndef CSPLINE_H
#define CSPLINE_H

#include "CObject.h"
#include "CPoint.h"
#include <memory>

//CAD上の点
class CSpline : public CObject
{
private:
    const static int DRAWING_LINE_SIZE = 3;
    const static int COLLISION_SIZE = 10;
protected:
    std::vector<Relative<Pos>> pos;

    virtual bool Create(Relative<Pos> pos,int index);
public:

    virtual Pos GetNear(const Pos& hand)const;
    virtual bool Draw(QPainter& painter)const;
    virtual bool Selecting();
    virtual bool isLocked();
    virtual bool Move(const Pos& diff);
    virtual int GetJointNum()const;
    virtual Pos GetJointPos(int index)const;


    CSpline();
    ~CSpline();
};


#endif // CSPLINE_H
