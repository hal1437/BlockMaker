#ifndef CFACE_H
#define CFACE_H

#include "CEdge.h"

//平面オブジェクト
class CFace : public CObject
{
    Q_OBJECT
public:
    static CFace* base[3];//正面,平面,右側面
    QVector<CEdge*> edges;//構成線

    //ポリゴン判定
    DEFINE_FLAG(Polygon,true)

public:
    //面が作成可能か
    static bool Creatable(QVector<CObject*> lines);
    virtual void DrawGL(Pos camera,Pos center)const;//三次元描画関数
    virtual bool DrawNormArrowGL()const;//三次元法線ベクトル描画関数

    virtual void ReorderEdges();//エッジ並び替え
    virtual bool isComprehension(Pos pos)const; //平面上かチェックする。
    virtual Pos  GetNorm()const ;               //法線ベクトル取得

    //エバリュエータ
    virtual void DefineMap2()const;//二次元エヴァリュエータ定義
    virtual Pos  GetPosFromUV      (double u,double v)const; //UV座標取得
    virtual Pos  GetPosFromUVSquare(double u,double v)const; //UV座標取得(全て直線と仮定して)

    //インデックス取得
    virtual CPoint* GetBasePoint()const;               //基準点取得
    virtual CEdge*  GetBaseEdge ()const;               //基準線取得
    virtual CPoint* GetPointSequence(int index)const;   //番号順点取得
    virtual CEdge*  GetEdgeSequence (int index)const;   //番号順線取得

    //子の操作
    virtual CEdge* GetEdge (int index);
    virtual CObject* GetChild(int index);
    virtual void     SetChild(int index,CObject* obj);
    virtual int      GetChildCount()const;

    //近接点
    virtual Pos GetNearPos (const Pos& hand)const;
    virtual Pos GetNearLine(const Pos& pos1,const Pos& pos2)const;

    //複製
    virtual CObject* Clone()const;

    CFace(QObject* parent=nullptr);
    ~CFace();
};

#endif // CFACE_H
