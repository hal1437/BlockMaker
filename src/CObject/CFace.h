#ifndef CFACE_H
#define CFACE_H

#include "CObject/CEdge.h"

//平面オブジェクト
class CFace : public CObject
{
public:
    QVector<CPoint*> corner;//角

public:
    bool isParadox()const;              //平面に矛盾がないか確認する。
    bool isComprehension(Pos pos)const; //平面上かチェックする。
    Pos  GetNorm()const ;               //法線ベクトル取得

    virtual bool Draw(QPainter& painter)const;//描画関数
    virtual bool Move(const Pos& diff);//移動関数


    //近接点
    virtual Pos GetNearPos (const Pos& hand)const;
    virtual Pos GetNearLine(const Pos& pos1,const Pos& pos2)const;

    CFace(QObject* parent=nullptr);
    ~CFace();
};

#endif // CFACE_H
