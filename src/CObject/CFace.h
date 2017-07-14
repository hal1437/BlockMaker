#ifndef CFACE_H
#define CFACE_H

#include "CObject/CEdge.h"
#include "CObject/CLine.h"

//平面オブジェクト
class CFace : public CObject
{
    Q_OBJECT
public:
    QVector<CEdge*> edges;//構成線
    bool is_polygon = true;

public:
    //面が作成可能か
    static bool Creatable(QVector<CObject*> lines);

    bool isPolygon()const{return this->is_polygon;}     //ポリゴン判定
    void SetPolygon(bool poly){this->is_polygon = poly;}

    virtual bool isComprehension(Pos pos)const; //平面上かチェックする。
    virtual Pos  GetNorm()const ;               //法線ベクトル取得

    virtual Pos  GetPosFromUV      (double u,double v)const; //UV座標取得
    virtual Pos  GetPosFromUVSquare(double u,double v)const; //UV座標取得(全て直線と仮定して)
    virtual CPoint* GetBasePoint()const;               //基準点取得
    virtual CEdge*  GetBaseEdge ()const;               //基準線取得
    virtual QVector<CPoint*> GetAllPoints()const;      //構成点取得
    virtual QVector<CEdge*>  GetAllEdges ()const;      //構成線取得
    virtual CPoint* GetPointSequence(int index)const;   //番号順点取得
    virtual CEdge*  GetEdgeSequence (int index)const;   //番号順線取得

    virtual bool Draw(QPainter& painter)const;//描画関数
    virtual bool DrawGL(Pos camera,Pos center)const;//三次元描画関数
    virtual bool DrawNormArrowGL()const;//三次元法線ベクトル描画関数
    virtual bool Move(const Pos& diff);//移動関数

    void ReorderEdges();//エッジ並び替え

    //近接点
    virtual Pos GetNearPos (const Pos& hand)const;
    virtual Pos GetNearLine(const Pos& pos1,const Pos& pos2)const;

    CFace(QObject* parent=nullptr);
    ~CFace();
};

#endif // CFACE_H
