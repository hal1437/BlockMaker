#ifndef SOLIDEDITCONTROLLER_H
#define SOLIDEDITCONTROLLER_H

#include <QObject>
#include <algorithm>
#include "Collision.h"
#include "CadModelCore.h"
#include "CObject/CFace.h"
#include "TimeDivider.h"


class SolidEditController:public QObject ,public CadModelCoreInterface
{
    Q_OBJECT

    //正面　 - XY平面
    //平面　 - XZ平面
    //右側面 - YZ平面
public:
    constexpr static int DEFAULT_FACE_LEGTH = 100;

    //CPoint* hang_point = nullptr;//最終生成点
    CFace* projection_face = nullptr; //投影面

    //正面を単位行列とした時の各面変換への行列
    Quat getConvertFrontToSide()const;
    Quat getConvertFrontToTop ()const;
    Quat getConvertSideToFront()const;
    Quat getConvertTopToFront ()const;
    Quat getConvertSideToTop  ()const;
    Quat getConvertTopToSide  ()const;

    double theta1 = 0; //角度1(縦方向)
    double theta2 = 0; //角度2(横方向)

    //正面を取得
    CFace* getFrontFace_impl(Quat convert,Quat invert)const;

public:

    CFace* getFrontFace()const;//正面
    CFace* getTopFace  ()const;//平面
    CFace* getSideFace ()const;//右側面
    Quat   getCameraMatrix()const;//カメラ変換行列取得

    void SetModel(CadModelCore *m);
    bool isSketcheing() const; //スケッチ中ならば

    CObject* getHangedObject(Pos center, Pos dir       ,double zoom_rate)const;
    CFace*   getHangedFace  (Pos center, Pos camera_pos,double zoom_rate)const;//直下面

    explicit SolidEditController(QObject *parent = 0);
    ~SolidEditController();

public slots:
    void Create3Face();  //三面を更新する。
    void CreateOrigin(); //原点を更新する。

signals:
    //カメラ注視点変更
    void ChangeCameraCenter(Pos center);
};

#endif // SOLIDEDITCONTROLLER_H
