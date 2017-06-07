#ifndef SOLIDEDITCONTROLLER_H
#define SOLIDEDITCONTROLLER_H

#include <QObject>
#include <algorithm>
#include "Collision.h"
#include "CadModelCore.h"
#include "CObject/CFace.h"

class SolidEditController:public QObject
{
    Q_OBJECT

    //正面　 - XY平面
    //平面　 - XZ平面
    //右側面 - YZ平面
public:
    constexpr static int DEFAULT_FACE_LEGTH = 100;
    CadModelCore* model;

    CPoint* hang_point = nullptr;//最終生成点

    //正面を単位行列とした時の各面変換への行列
    Quat getConvertFrontToSide()const;
    Quat getConvertFrontToTop ()const;
    Quat getConvertSideToFront()const;
    Quat getConvertTopToFront ()const;
    Quat getConvertSideToTop  ()const;
    Quat getConvertTopToSide  ()const;

    CFace* base[3];//正面,平面,右側面

    //正面を取得
    CFace* getFrontFace_impl(Quat invert)const;

public:
    void setModel(CadModelCore* model){this->model = model;}

    CFace* getFrontFace()const;//正面
    CFace* getTopFace  ()const;//平面
    CFace* getSideFace ()const;//右側面

    CObject* getHangedObject(Pos center, Pos dir)const;
    CFace* getHangedFace(Pos center, Pos dir)const;//直下面

    explicit SolidEditController(QObject *parent = 0);
    ~SolidEditController();

public slots:
    void Refresh3Face();//三面を更新する。

};

#endif // SOLIDEDITCONTROLLER_H
