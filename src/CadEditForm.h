#ifndef CADEDITFORM_H
#define CADEDITFORM_H

#include <QWidget>
#include <QDebug>
#include <QMouseEvent>
#include <QListWidget>
#include <QTreeWidget>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include <iostream>
#include <random>
#include <queue>
#include <utility>
#include "CObject/CObject.h"
#include "CObject/CPoint.h"
#include "CObject/CLine.h"
#include "CObject/CRect.h"
#include "CObject/CBlock.h"
#include "CBoxDefineDialog.h"
#include "SmartDimension.h"
#include "SmartDimensionDialog.h"
#include "Restraint.h"
#include "ExportDialog.h"

namespace Ui {
class CadEditForm;
}

enum CEnum{
    Edit,
    Dot,
    Line,
    Arc,
    Rect,
    Spline,
};

class CadEditForm : public QWidget
{
    Q_OBJECT
private:
    Ui::CadEditForm *ui; //UI

    QVector<CObject*>         objects;         //オブジェクト
    QVector<SmartDimension*>  dimensions;      //寸法
    QVector<Restraint*>       restraints;      //拘束
    QVector<CBlock>           blocks;          //立体
    int                       selecting_block; //選択物体

    double scale  = 1.0f;     //拡大率
    Pos translate = Pos(0,0); //平行移動量
    Pos zoom_piv;  //ズーム支点

    bool release_flag = false;
    bool move_flag    = false;   //移動フラグ
    bool shift_pressed = false;  //シフトフラグ
    bool ctrl_pressed  = false;  //コントロールフラグ
    CEnum state       = Edit;    //生成種類
    CObject* make_obj = nullptr;
    CPoint*  origin   = nullptr;
    int creating_count=0;
    QVector<CObject*> log;

protected:
    void mousePressEvent  (QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void paintEvent       (QPaintEvent* event);
    void wheelEvent       (QWheelEvent * event);
    void mouseMoveEvent   (QMouseEvent*  event); //マウス移動イベントハンドラ
    void resizeEvent      (QResizeEvent* event); //リサイズイベントハンドラ

public:
    void keyPressEvent    (QKeyEvent* event);
    void keyReleaseEvent  (QKeyEvent* event);

    void Escape();//作成破棄

    void AddObject     (CObject* obj);      //オブジェクト追加
    void RemoveObject  (CObject* obj);      //オブジェクト削除
    void CompleteObject(CObject* make_obj); //非端点を追加
    double GetScale()const;                 //スケール取得
    Pos    GetTranslate()const;             //平行移動量取得
    void    MovedMouse(QMouseEvent *event, CObject *under_object); //マウス移動シグナル

    void Zoom(double scale,Pos local_piv); //ズーム
    void Move(Pos local_diff);             //平行移動

    Pos ConvertLocalPos(Pos pos)const; //仮想座標へ変換
    Pos ConvertWorldPos(Pos pos)const; //表示座標へ変換

    void MargePoints();//幾何拘束:マージ

signals:
    void ToggleConflict(bool conflict); //競合シグナル変更
    void ScaleChanged(double value);    //拡大倍率変更
    void RquireRefreshUI();             //UI更新
    void MouseMoved(Pos pos);            //マウス移動


public slots:
    CObject* getHanged(); //直下オブジェクト選定
    void SetScale(double scale);  //スケールセット
    void SetTranslate(Pos trans); //並行移動セット

    void MakeObject();
    bool MakeJoint(CObject *obj);           //ジョイント作成

    void MakeRestraint(RestraintType type); //拘束作成
    void MakeSmartDimension();              //寸法設定
    bool MakeBlock();                       //物体生成
    void RefreshRestraints();               //拘束更新

    void ApplyObjectList(QTreeWidget *list); //オブジェクトリスト選択
    void DrawObjectList (QTreeWidget *list); //オブジェクトリスト描画
    void ApplyCBoxList  (QListWidget *list); //ブロックリスト選択
    void DrawCBoxList   (QListWidget *list); //ブロックリスト描画
    void ConfigureBlock (QListWidgetItem*);  //ブロック再編集

    void SetState(CEnum state); //生成種類設定

    void ResetAllExpantion();//拡大、移動リセット
    void Export(); //出力

public:
    explicit CadEditForm(QWidget *parent = 0);
    ~CadEditForm();

};

#endif // CADEDITFORM_H
