#ifndef CLINE_H
#define CLINE_H
#include "CObject.h"
#include "CPoint.h"
#include <memory>

//CAD上の点
class CLine : public CObject
{
private:
    const static int COLLISION_SIZE = 5;
protected:
    CPoint* pos[2];

    virtual bool Create(CPoint * pos,int index);
public:

    virtual Pos GetNear(const Pos& hand)const;
    virtual bool Draw(QPainter& painter,QTransform trans)const;
    virtual bool isSelectable()const;
    virtual void Lock(bool lock);
    virtual bool Move(const Pos& diff);
    virtual int GetJointNum()const;
    virtual Pos GetJointPos(int index)const;
    virtual CPoint* GetJoint(int index);
    virtual std::vector<CObject*> GetChild();

    double DistanceToPoint(const Pos& pos)const;


    CLine();
    ~CLine();
};


#endif // CLINE_H