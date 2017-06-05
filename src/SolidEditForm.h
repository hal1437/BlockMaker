#ifndef SOLIDEDITFORM_H
#define SOLIDEDITFORM_H

#include <QWidget>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <GLUT/glut.h>
#include "MakeObjectController.h"
#include "SolidEditController.h"
#include "TimeDivider.h"
#include "CadModelCore.h"
#include "CObject/CFace.h"

namespace Ui {
class SolidEditForm;
}

class SolidEditForm : public QOpenGLWidget,public CadModelCoreInterface
{
    Q_OBJECT

    const int SENSITIVITY = 100;
private:
    SolidEditController* controller;
    MakeObjectController* make_controller;

    Pos mouse_pos;  //マウス座標
    Pos screen_pos; //スクリーン座標

    MAKE_OBJECT state = MAKE_OBJECT::Edit;    //生成種類
    CFace* sketch_face = nullptr;  //スケッチ平面
    Quat sketch_mat;   //スケッチ生成行列

    Pos camera; //カメラ位置
    Pos center; //カメラ注意点
    double round = 1;  //半径
    double theta1 = 0; //角度1(縦方向)
    double theta2 = 0; //角度2(横方向)

    bool shift_pressed = false; //shiftボタン
    bool ctrl_pressed  = false; //ctrlボタン
    Pos drag_base;   //ドラッグ起点
    Pos first_click; //クリック起点

private:

    void MakeObject(); //オブジェクト生成

    void StartSketch(CFace* face);  //スケッチ開始
    bool isSketcheing();            //スケッチ中
    CFace*   GetHangedFace  ();     //直下面を取得
    CObject* GetHangedObject();     //直下オブジェクトを取得
    void ColorSelect(CObject* obj); //オブジェクト色選択

public:
    //カメラ方向セット
    void setCameraRotate(double theta1, double theta2);

public:
    void keyPressEvent    (QKeyEvent *event);
    void keyReleaseEvent  (QKeyEvent *event);
    void mousePressEvent  (QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent   (QMouseEvent *event);
    void wheelEvent       (QWheelEvent *event);

    void SetModel(CadModelCore* model);
    void initializeGL();        //  OpenGL 初期化
    void resizeGL(int, int);    //  ウィジットリサイズ時のハンドラ
    void paintGL();             //  描画処理
public slots:

    void SetState(MAKE_OBJECT state); //生成種類設定
    void CEdgeChanged(QVector<CEdge*> e);
    void CBlockChanged(QVector<CBlock*> e);

public:
    explicit SolidEditForm(QWidget *parent = 0);
    ~SolidEditForm();

private:
    Ui::SolidEditForm *ui;
};

#endif // SOLIDEDITFORM_H
