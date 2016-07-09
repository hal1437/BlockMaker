#ifndef SMARTDIMENSION_H
#define SMARTDIMENSION_H

#include <vector>
#include <QMessageBox>
#include <algorithm>
#include "CObject.h"
#include "CPoint.h"
#include "CLine.h"
#include "CArc.h"
#include "Restraint.h"


class SmartDimension
{
public:
    enum DimensionType{
        none,         //無効
        length,       //1線距離
        distance,     //2点距離
        distanceLine, //点距離
        angle,        //角度
        radius,       //半径
        concurrent,   //並行
    };

private:
    DimensionType type;  //高速タイプ
    double value = 1.0;  //測定値
    CObject* target[2];  //対象
    bool X_type = false; //X軸成分
    bool Y_type = false; //Y軸成分

private:

    void DrawString(QPainter& painter,const Pos& pos,const QString& str,double angle)const;
    void DrawArrow (QPainter& painter,const Pos& pos,const Pos& rote,double angle,double length = 15)const;

public:

    static DimensionType GetDimensionType(CObject* obj1 = nullptr,CObject* obj2 = nullptr);

    void   SetValue(double value);
    double GetValue()const;

    bool SetTarget(CObject* obj1,CObject* obj2);
    CObject* GetTarget(int index)const;
    std::vector<Restraint *> MakeRestraint();

    bool Draw(QPainter& painter)const;

    SmartDimension();
};

#endif // SMARTDIMENSION_H
