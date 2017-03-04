#ifndef CBLOCK_H
#define CBLOCK_H

#include <vector>
#include <QPainter>
#include "CObject.h"
#include "CLine.h"
#include "CArc.h"
#include "CSpline.h"
#include "CRect.h"

//面の方向
enum BoundaryDir{
    Top,
    Right,
    Left,
    Bottom,
    Front,
    Back,
};

//面の種類
enum BoundaryType{
    Empty,
    Patch,
    Wall,
    SymmetryPlane,
    Cyclic,
    CyclicAMI,
    Wedge,
};
//格子点間隔の種類
enum GradingType{
    SimpleGrading,
    EdgeGrading,
};


class CBlock{
private:
    QVector<CObject*> lines;
public:
    BoundaryType boundery[6];
    QString name[6];

    GradingType grading;
    QVector<double> grading_args;
public:

    static bool Creatable(QVector<CObject*> values);//BOX定義可能か
    bool isParadox()const;//矛盾確認

    //描画
    void Draw(QPainter& painter)const;

    void SetNodeAll(QVector<CObject*> lines);
    void SetNode(int index,CObject* line);
    CObject* GetNode(int index)const;
    QVector<Pos> GetVerticesPos()const;


    CBlock();
    CBlock(QVector<CObject*> lines);
    ~CBlock();
};

#endif // CBLOCK_H
