#ifndef CADEDITFORM_H
#define CADEDITFORM_H

#include <QWidget>
#include <QDebug>
#include <QMouseEvent>
#include <iostream>
#include "CObject.h"
#include "CPoint.h"
#include "CLine.h"
#include "SmartDimension.h"
#include "SmartDimensionDialog.h"
#include "Restraint.h"

namespace Ui {
class CadEditForm;
}

class CadEditForm : public QWidget
{
    Q_OBJECT
private:
    QVector<CObject*>         objects; //物体
    QVector<SmartDimension*>  dimensions; //寸法
    QVector<Restraint*>       restraints; //拘束
    double scale = 1.0f;
    Pos transform = Pos(0,0);
public:

    void AddObject(CObject* obj);
    void RemoveObject(CObject* obj);
    double GetScale()const;
    Pos    GetTransform()const;

    void paintEvent    (QPaintEvent* event); //描画イベントハンドラ
    void mouseMoveEvent(QMouseEvent* event); //マウス移動


    explicit CadEditForm(QWidget *parent = 0);
    ~CadEditForm();

private:
    Ui::CadEditForm *ui;

signals:
    void MovedMouse(QMouseEvent *event, CObject *under_object);

public slots:
    CObject* Selecting();
    void SetScale(double scale);
    void SetTransform(Pos trans);
    void MakeSmartDimension();
    void RefreshRestraints();

};

#endif // CADEDITFORM_H
