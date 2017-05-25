#ifndef SOLIDEDITFORM_H
#define SOLIDEDITFORM_H

#include <QWidget>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <gl.h>
#include <glu.h>
#include "SolidEditController.h"
#include "TimeDivider.h"
#include "CadModelCore.h"

namespace Ui {
class SolidEditForm;
}

enum EDIT_STATE{
    FREE   , //カメラ自由旋回モード
    SKETCH , //二次元作業中
};

class SolidEditForm : public QOpenGLWidget
{
    Q_OBJECT

    const int SENSITIVITY = 100;
private:
    CadModelCore* model;
    SolidEditController* controller;

    Pos mouse_pos;  //マウス座標
    Pos click_base; //ドラッグ保持点

    EDIT_STATE status; //作業状態
    Quat sketch_mat;   //スケッチ生成行列

    Pos camera; //カメラ位置
    Pos center; //カメラ注意点
    double round = 1;//半径
    double theta1 = 0; //角度1(縦方向)
    double theta2 = 0; //角度2(横方向)

private:

    //選択
    void MouseSelect();

public:
    //カメラ方向セット
    void setCameraRotate(double theta1, double theta2);

public:
    void keyPressEvent    (QKeyEvent *event);
    void mousePressEvent  (QMouseEvent *event);
    void mouseMoveEvent   (QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent       (QWheelEvent *event);

    void setModel(CadModelCore *model);
    void initializeGL();        //  OpenGL 初期化
    void resizeGL(int, int);    //  ウィジットリサイズ時のハンドラ
    void paintGL();             //  描画処理
public slots:

    void CEdgeChanged(QVector<CEdge*> e);
    void CBlockChanged(QVector<CBlock*> e);

public:
    explicit SolidEditForm(QWidget *parent = 0);
    ~SolidEditForm();

private:
    Ui::SolidEditForm *ui;
};

#endif // SOLIDEDITFORM_H
