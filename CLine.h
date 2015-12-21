#ifndef CLINE_H
#define CLINE_H
#include "CObject.h"
#include "CPoint.h"
#include <memory>

//CAD上の点
class CLine : public CObject
{
protected:
    Relative<Pos> pos[2];

    virtual bool Create(Relative<Pos> pos,int index);
public:

    virtual bool Draw(QPainter& painter)const;
    virtual bool Selecting();
    virtual bool isLocked();

    CLine();
    ~CLine();
};


#endif // CLINE_H
