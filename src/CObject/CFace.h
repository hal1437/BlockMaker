#ifndef CFACE_H
#define CFACE_H

#include "CObject/CEdge.h"

//平面オブジェクト
class CFace : public CObject
{
public:
    QVector<CEdge*> edges;//構成線
    bool is_polygon = true;

public:
    //面が作成可能か
    static bool Creatable(QVector<CObject*> lines);

    bool isPolygon()const{return this->is_polygon;}
    void SetPolygon(bool poly){this->is_polygon = poly;}

    bool isParadox()const;              //平面に矛盾がないか確認する。
    bool isComprehension(Pos pos)const; //平面上かチェックする。
    Pos  GetNorm()const ;               //法線ベクトル取得
    QVector<CPoint*> GetPoint(); //構成点取得
    QVector<CPoint*> GetPoint()const; //構成点取得

    virtual bool Draw(QPainter& painter)const;//描画関数
    virtual bool DrawGL(Pos camera,Pos center)const;//三次元描画関数
    virtual bool Move(const Pos& diff);//移動関数


    //近接点
    virtual Pos GetNearPos (const Pos& hand)const;
    virtual Pos GetNearLine(const Pos& pos1,const Pos& pos2)const;

    CFace(QObject* parent=nullptr);
    ~CFace();
};

#endif // CFACE_H
