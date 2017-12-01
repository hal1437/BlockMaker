#ifndef SOLIDEDITFORM_H
#define SOLIDEDITFORM_H

#include <QWidget>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QGLWidget>
#include "MakeObjectController.h"
#include "SolidEditController.h"
#include "TimeDivider.h"
#include "CadModelCore.h"
#include "CadModelMenu.h"
#include "CObject/CFace.h"

namespace Ui {
class SolidEditForm;
}

class SolidEditForm : public QOpenGLWidget ,public CadModelCoreInterface
{
    Q_OBJECT

    const int    SENSITIVITY        = 100;      //マウス感度
    const double CAMERA_ANGLE_LIMIT = M_PI/720; //角度限界値
private:
    SolidEditController* controller;
    MakeObjectController* make_controller;
    CadModelMenu menu;

    Pos mouse_pos;  //マウス座標
    Pos screen_pos; //スクリーン座標

    MAKE_OBJECT state = MAKE_OBJECT::Edit;    //生成種類

    Pos camera; //カメラ位置
    Pos center; //カメラ注意点
    double round = 1;  //半径

    bool shift_pressed = false; //shiftボタン
    bool ctrl_pressed  = false; //ctrlボタン
    Pos drag_base;   //ドラッグ起点
    Pos first_click; //クリック起点

private:

    void MakeObject(); //オブジェクト生成

    CFace*   GetHangedFace  ();     //直下面を取得
    CObject* GetHangedObject();     //直下オブジェクトを取得
    void ColorSelect(CObject* obj); //オブジェクト色選択

public:
    void keyPressEvent    (QKeyEvent *event);
    void keyReleaseEvent  (QKeyEvent *event);
    void mousePressEvent  (QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent   (QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void wheelEvent       (QWheelEvent *event);

    void SetModel(CadModelCore* model);
    void initializeGL();              //  OpenGL初期化
    void resizeGL(int, int);          //  ウィジットリサイズ時のハンドラ
    void paintGL();                   //  描画処理
    void paintObject(CObject* obj, QVector<double> color, int tick); //  選択したオブジェクトを描画する

public slots:

    void StartSketch(CFace* face);
    void SetCameraRotate(double theta1,double theta2);
    void SetCameraCenter(Pos point);
    void SetZoomRate(double round);
    void SetState(MAKE_OBJECT state); //生成種類設定
    void CEdgeChanged(QVector<CEdge*> e);
    void CBlockChanged(QVector<CBlock*> e);

signals:
    void MousePosChanged(Pos);

public:
    explicit SolidEditForm(QWidget *parent = 0);
    ~SolidEditForm();

private:
    Ui::SolidEditForm *ui;
};

#endif // SOLIDEDITFORM_H
