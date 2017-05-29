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
#include <fstream>
#include <random>
#include <queue>
#include <utility>
#include "CadModelCore.h"
#include "Dialog/CBoxDefineDialog.h"
#include "Dialog/SmartDimensionDialog.h"
#include "Dialog/ExportDialog.h"
#include "Dialog/GridFilter.h"

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

class CadEditForm : public QWidget ,public CadModelCoreInterface
{
    Q_OBJECT

private:
    Ui::CadEditForm *ui; //UI

    Pos mouse_pos;//マウス位置
    CEdge* creating;          //作成途中オブジェクト
    QVector<CObject*> selected; //選択オブジェクト

    int selecting_block; //選択物体

    CEnum   state      = Edit;    //生成種類
    CPoint* hang_point = nullptr; //生成支点
    int make_count     = COMPLETE; //生成管理フラグ

    double depth  = 0.0f;     //視点の深さ
    double scale  = 1.0f;     //拡大率
    Pos zoom_piv  = Pos(0,0,0); //拡大支点
    Pos translate = Pos(0,0,0); //平行移動量

    bool release_flag  = false;  //クリックトグル化フラグ
    bool move_flag     = false;  //移動フラグ
    bool shift_pressed = false;  //シフトボタンフラグ
    bool ctrl_pressed  = false;  //コントロールフラグ

    GridFilter filter; //フィルター
    QVector<CObject*> log; //ログ

protected:
    void mousePressEvent  (QMouseEvent*  event); //マウス押しイベントハンドラ
    void mouseReleaseEvent(QMouseEvent*  event); //マウス離しイベントハンドラ
    void mouseMoveEvent   (QMouseEvent*  event); //マウス移動イベントハンドラ
    void paintEvent       (QPaintEvent*  event); //再描画イベントハンドラ
    void wheelEvent       (QWheelEvent*  event); //ホイールイベントハンドラ
    void resizeEvent      (QResizeEvent* event); //リサイズイベントハンドラ

public:
    void keyPressEvent    (QKeyEvent* event); //キーボード押しイベント
    void keyReleaseEvent  (QKeyEvent* event); //キーボード離しイベント


    Pos               getMousePos()const{return this->mouse_pos;} //マウス座標取得
    QVector<CObject*> getSelected()const{return this->selected; } //選択オブジェクト取得
    CObject*          getHanged  ()const; //直下オブジェクト選定

    void Escape();//作成破棄

    void Zoom     (double scale,Pos local_piv); //ズーム
    void Translate(Pos local_diff);             //平行移動

    Pos ConvertLocalPos(Pos pos)const; //仮想座標へ変換
    Pos ConvertWorldPos(Pos pos)const; //表示座標へ変換

    void MergePoints();//幾何拘束:マージ

signals:
    void ToggleConflict(bool conflict); //競合シグナル変更
    void ScaleChanged(double value);    //拡大倍率変更
    void DepthChanged(double value);    //深さ変更
    void RequireRefreshUI();             //UI更新
    void MouseMoved(Pos pos);            //マウス移動
    void RequireRefreshSolidUI(QVector<CEdge*> edges,QVector<CBlock> blocks);



public slots:


    void SetDepth(double depth);  //深さ視点セット
    void SetScale(double scale);  //スケールセット
    void SetTranslate(Pos trans); //並行移動セット
    void SetGridFilterStatus(double x,double y); //フィルターグリッドセット


    void MakeObject();
    CREATE_RESULT MakeJoint(CObject* obj);           //ジョイント作成

    void MakeRestraint(RestraintType type); //拘束作成
    void MakeSmartDimension();              //寸法設定
    bool MakeBlock();                       //物体生成
    void RefreshRestraints();               //拘束更新

    void ImportObjectList(QTreeWidget *list); //オブジェクトリスト入力
    void ExportObjectList (QTreeWidget *list); //オブジェクトリスト出力
    void ImportCBoxList  (QListWidget *list); //ブロックリスト入力
    void ExportCBoxList   (QListWidget *list); //ブロックリスト出力
    void ConfigureBlock (QListWidgetItem*);  //ブロック再編集

    void SetState(CEnum state); //生成種類設定

    void ResetAllExpantion();//拡大、移動リセット
    void Export(); //出力
    void Save();//保存
    void Load();//読み込み
    void MovedMouse(QMouseEvent *event, CObject* under_object); //マウス移動シグナル

public:
    explicit CadEditForm(QWidget *parent = 0);
    ~CadEditForm();

};

#endif // CADEDITFORM_H
