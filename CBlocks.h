#ifndef CBLOCKS_H
#define CBLOCKS_H

#include <vector>
#include "CObject.h"
#include "CLine.h"
#include "CArc.h"
#include "CSpline.h"

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


class CBlocks{
private:
    std::vector<CObject*> lines;
public:
    BoundaryType boundery[6];
    QString name[6];
    std::vector<int> vertices[6];

    GradingType grading;
    std::vector<double> grading_args;
public:

    //BOX定義可能か
    static bool Creatable(std::vector<CObject*> values);

       //矛盾平面
    bool isParadox()const;

    //ノード
    void SetNodeAll(std::vector<CObject*> lines);
    void     SetNode(int index,CObject* line);
    CObject* GetNode(int index)const;

    CBlocks();
    CBlocks(std::vector<CObject*> lines);
    ~CBlocks();
};

#endif // CBLOCKS_H
