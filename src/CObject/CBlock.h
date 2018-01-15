#ifndef CBLOCK_H
#define CBLOCK_H

#include "CObject.h"
#include "CLine.h"
#include "CArc.h"
#include "CSpline.h"
#include "CFace.h"

class CBlock : public CObject{
    Q_OBJECT
public:
    QList<CFace*> faces;

private:
    DEFINE_FLAG(VisibleDetail,true)//分割フレーム表示

private:
    //旧バージョン
    //const int edge_comb[12][2] = {{0,1},{1,2},{3,2},{0,3},{4,5},{5,6},
    //                              {7,6},{4,7},{0,4},{1,5},{2,6},{3,7}};

    const int edge_comb[12][2] = {{0,1},{3,2},{7,6},{4,5},
                                  {0,3},{1,2},{5,6},{4,7},
                                  {0,4},{1,5},{2,6},{3,7}};

    //各軸長さ取得関数
    double GetLength_impl(Quat convert);

public:
    virtual QString DefaultClassName(){return "CBlock";}
    //立体が作成可能か
    static bool Creatable(QList<CObject* > values);
    virtual void Create(QList<CFace*> faces);     //作成

    void DrawGL(Pos camera,Pos center)const; //三次元描画関数

    //インデックス取得
    virtual CPoint* GetBasePoint()const;  //基準点取得
    virtual CEdge*  GetBaseEdge()const;   //基準線取得
    virtual QList<CPoint*> GetAllPoints()const; //構成点取得
    virtual QList<CEdge* > GetAllEdges()const;  //構成線取得
    virtual QList<CFace* > GetAllFaces()const;  //構成面取得
    virtual CPoint* GetPointSequence(int index) const;//番号順点取得
    virtual CEdge*  GetEdgeSequence (int index) const;//番号順線取得
    virtual bool isEdgeReverse(int index);//番号のエッジが反転しているか

    //子の操作
    virtual CFace*   GetFace (int index);
    virtual CObject* GetChild(int index);
    virtual void     SetChild(int index,CObject* obj);
    virtual int      GetChildCount()const;

    //分割点取得(辺の番号)
    Pos GetDivisionPoint(int edge_index, int count_index)const;

    double GetLengthX(); //X方向幅取得
    double GetLengthY(); //Y方向幅取得
    double GetLengthZ(); //Z方向幅取得

    //近接点
    Pos GetNearPos (const Pos& hand)const;
    Pos GetNearLine(const Pos& pos1,const Pos& pos2)const;

public:
    virtual CObject* Clone()const;

    CBlock(QObject* parent=nullptr);
    virtual ~CBlock();

};

#endif // CBLOCK_H
