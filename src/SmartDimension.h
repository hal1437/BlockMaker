#ifndef SMARTDIMENSION_H
#define SMARTDIMENSION_H

#include <vector>
#include <QMessageBox>
#include <algorithm>
#include "CObject/CObject.h"
#include "CObject/CPoint.h"
#include "CObject/CLine.h"
#include "CObject/CArc.h"
#include "Restraint.h"


class SmartDimension
{
public:
    enum DimensionType{
        none,         //無効
        length,       //線の長さ
        distance,     //2点間距離
        distanceLine, //線と点の距離
        angle,        //角度
        radius,       //半径
        concurrent,   //線同士の距離
    };

private:
    DimensionType type;  //拘束タイプ
    double value = 1.0;  //設定値
    CObject* target[2];  //対象
    bool X_type = false; //X軸成分
    bool Y_type = false; //Y軸成分

private:

    void DrawString(QPainter& painter,const Pos& pos,const QString& str,double angle)const;
    void DrawArrow (QPainter& painter,const Pos& pos,const Pos& rote,double angle,double length = 15)const;

public:

    static DimensionType GetDimensionType(CObject* obj1 = nullptr,CObject* obj2 = nullptr);

    //現在の状態の値を取得
    double currentValue()const;

    //値の入出力
    void   SetValue(double value);
    double GetValue()const;

    //ターゲット設定
    bool SetTarget(CObject* obj1,CObject* obj2);
    //XY軸設定
    bool SetXYType(bool x,bool y);

    //ターゲット取得
    CObject* GetTarget(int index)const;
    //拘束作成
    std::vector<Restraint *> MakeRestraint();

    bool Draw(QPainter& painter,QTransform trans)const;

    SmartDimension();
};

#endif // SMARTDIMENSION_H
