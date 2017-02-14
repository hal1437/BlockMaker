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
    virtual bool Create(CPoint * hand, int index);
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


    CRect();
    ~CRect();
};


#endif // CRECT_H
