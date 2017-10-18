#ifndef CSTL_H
#define CSTL_H
#include "CPoint.h"
#include "CLine.h"
#include <QFile>

class CStl :public CObject
{
    Q_OBJECT

public:
    QString name;           //STL名
    QVector<CPoint*> points;//点群
    QVector<CEdge* > edges; //線群
public:
    static CStl* CreateFromFile(QString filepath);

public:
    virtual void DrawGL(Pos camera,Pos center)const; //三次元描画関数

    //中間点操作
    virtual CObject* GetChild(int index);
    virtual void     SetChild(int index,CObject* obj);
    virtual int      GetChildCount()const;

    //近接点
    virtual Pos GetNearPos (const Pos& hand)const;
    virtual Pos GetNearLine(const Pos& pos1,const Pos& pos2)const;

    //複製
    virtual CObject* Clone()const ;

    CStl();
};

#endif // CSTL_H
