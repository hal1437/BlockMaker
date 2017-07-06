#ifndef CBLOCK_H
#define CBLOCK_H

#include <vector>
#include <QPainter>
#include "CObject.h"
#include "CLine.h"
#include "CArc.h"
#include "CSpline.h"
#include "CFace.h"

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
    Empty         = 0,
    Patch         = 1,
    Wall          = 2,
    SymmetryPlane = 3,
    Cyclic        = 4,
    CyclicAMI     = 5,
    Wedge         = 6,
    None          = 7,//連続
};
//格子点間隔の種類
enum GradingType{
    EmptyGrading = 0,
    SimpleGrading = 1,
    EdgeGrading = 2,
};


class CBlock : public CObject{
    Q_OBJECT
public:
    QVector<CFace*> faces;
public:
    BoundaryType boundery[6]; // 境界タイプ
    QString name[6];          // 境界名
    int div[3];               // 分割数
    GradingType grading;      // 分割間隔タイプ
    QVector<double> grading_args; // 分割パラメータ
private:
    QVector<QVector<Pos>> div_pos; //分割位置 (エッジ番号,分割数)
    bool visible_Mesh = true; //分割フレーム表示

private:
    //各軸長さ取得関数
    double GetLength_impl(Quat convert);

public:
    //分割点取得(辺の番号)
    Pos GetDivisionPoint(int edge_index, int count_index)const;

    //幅取得
    double GetLengthX();
    double GetLengthY();
    double GetLengthZ();

    bool Draw  (QPainter& painter)const;     //描画関数
    bool DrawGL(Pos camera,Pos center)const; //三次元描画関数
    bool Move  (const Pos& diff);            //移動関数

    bool isVisibleMesh()const;//フレーム表示
    void VisibleMesh(bool flag);

    //近接点
    Pos GetNearPos (const Pos& hand)const;
    Pos GetNearLine(const Pos& pos1,const Pos& pos2)const;

public:

    static bool Creatable(QVector<CObject* > values);//BOX定義可能か
    bool isParadox()const;//矛盾確認

    CPoint* GetBasePos()const;//基準点取得
    QVector<CPoint*> GetAllPos()const;
    QVector<CEdge* > GetAllEdges()const;
    CPoint* GetClockworksPos (int index) const;//番号順点取得
    CEdge* GetClockworksEdge(int index) const;//番号順線取得

    CBlock(QObject* parent=nullptr);
    virtual ~CBlock();

public slots:
    void RefreshDividePoint();//div_pos再計算
    void ReorderEdges();
};

#endif // CBLOCK_H
