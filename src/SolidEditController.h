#ifndef SOLIDEDITCONTROLLER_H
#define SOLIDEDITCONTROLLER_H

#include <QObject>
#include <algorithm>
#include "CadModelCore.h"

struct Face{
    Pos corner[4];
};

class SolidEditController:public QObject
{
    Q_OBJECT

    //正面　 - XY平面
    //平面　 - XZ平面
    //右側面 - YZ平面
public:
    constexpr static int DEFAULT_FACE_LEGTH = 100;
    CadModelCore* model;

    //正面を単位行列とした時の各面変換への行列
    Matrix<double,3,3> getConvertFrontToSide()const;
    Matrix<double,3,3> getConvertFrontToTop ()const;
    Matrix<double,3,3> getConvertSideToFront()const;
    Matrix<double,3,3> getConvertTopToFront ()const;
    Matrix<double,3,3> getConvertSideToTop  ()const;
    Matrix<double,3,3> getConvertTopToSide  ()const;


    //正面を取得
    Face getFrontFace_impl(Matrix<double,3,3> convert, Matrix<double, 3, 3> invert)const;

public:
    void setModel(CadModelCore* model){this->model = model;}


    Face getFrontFace()const;//正面
    Face getTopFace  ()const;//平面
    Face getSideFace ()const;//右側面


    explicit SolidEditController(QObject *parent = 0);
    ~SolidEditController();
};

#endif // SOLIDEDITCONTROLLER_H
