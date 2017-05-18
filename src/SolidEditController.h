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

public:
    constexpr static int DEFAULT_FACE_LEGTH = 100;
    CadModelCore* model;

public:
    void setModel(CadModelCore* model){this->model = model;}


    Face getFrontFace()const;//正面
    Face getTopFace  ()const;//平面
    Face getSideFace ()const;//右側面


    explicit SolidEditController(QObject *parent = 0);
    ~SolidEditController();
};

#endif // SOLIDEDITCONTROLLER_H
