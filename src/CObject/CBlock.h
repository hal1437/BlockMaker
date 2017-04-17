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
    EmptyGrading,
    SimpleGrading,
    EdgeGrading,
};


class CBlock{
private:
    QVector<CObject*> lines;
public:
    BoundaryType boundery[6]; // 境界タイプ
    QString name[6];          // 境界名

    int div[3];               // 分割数
    double depth;             //深さ
    GradingType grading;          // 分割間隔タイプ
    QVector<double> grading_args; // 分割パラメータ

    //分割点取得(辺の番号)
    Pos GetDivisionPoint(int edge_index, int count_index)const;

    //幅取得
    double GetWidth();
    //高さ取得
    double GetHeight();

public:

    static bool Creatable(QVector<CObject*> values);//BOX定義可能か
    bool isParadox()const;//矛盾確認

    //描画
    void Draw(QPainter& painter)const;

    void SetNodeAll(QVector<CObject*> lines);
    void SetNode(int index,CObject* line);
    CObject* GetNode(int index)const;
    QVector<Pos> GetVerticesPos()const;
    Pos GetClockworksPos(int index) const;//時計回り番号取得


    CBlock();
    CBlock(QVector<CObject*> lines);
    ~CBlock();
};

#endif // CBLOCK_H
