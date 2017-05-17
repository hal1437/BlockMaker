#ifndef SOLIDEDITFORM_H
#define SOLIDEDITFORM_H

#include <QWidget>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <gl.h>
#include <glu.h>
#include "CadModelCore.h"

namespace Ui {
class SolidEditForm;
}

class SolidEditForm : public QOpenGLWidget
{
    Q_OBJECT

    const int SENSITIVITY = 100;
private:
    CadModelCore* model;

    Pos click_base; //ドラッグ保持点

    Pos camera; //カメラ位置
    Pos center; //カメラ注意点
    double round = 10;//半径
    double theta1 = 45.0; //角度1
    double theta2 = 45.0; //角度2

public:

public:
    void mousePressEvent  (QMouseEvent *event);
    void mouseMoveEvent   (QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent       (QWheelEvent *event);

    void setModel(CadModelCore *model);
    void initializeGL();        //  OpenGL 初期化
    void resizeGL(int, int);    //  ウィジットリサイズ時のハンドラ
    void paintGL();             //  描画処理

public:
    explicit SolidEditForm(QWidget *parent = 0);
    ~SolidEditForm();

private:
    Ui::SolidEditForm *ui;
};

#endif // SOLIDEDITFORM_H
