#ifndef CADEDITFORM_H
#define CADEDITFORM_H

#include <QWidget>
#include <QDebug>
#include <QMouseEvent>
#include "CObject.h"

namespace Ui {
class CadEditForm;
}

class CadEditForm : public QWidget
{
    Q_OBJECT
private:
    QVector<CObject*> objects;
public:

    void AddObject(CObject* obj);
    void RemoveObject(CObject* obj);

    void paintEvent(QPaintEvent * event);    //描画イベントハンドラ
    void mouseMoveEvent   (QMouseEvent* event);     //マウス移動


    explicit CadEditForm(QWidget *parent = 0);
    ~CadEditForm();

private:
    Ui::CadEditForm *ui;

signals:
    void MovedMouse();

public slots:
    CObject* Selecting();

};

#endif // CADEDITFORM_H
