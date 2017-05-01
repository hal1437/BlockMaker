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
#include "CObject/CObject.h"
#include "CObject/CPoint.h"
#include "CObject/CLine.h"
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

    QVector<CEdge*>           edges;           //エッジ
    QVector<CBlock>           blocks;          //ブロック
    QVector<SmartDimension*>  dimensions;      //寸法
    QVector<Restraint*>       restraints;      //拘束
    int                       selecting_block; //選択物体

    CEnum   state      = Edit;    //生成種類
    CEdge*  make_obj   = nullptr; //生成途中オブジェクト
    CPoint* origin     = nullptr; //原点
    CPoint* hang_point = nullptr; //生成支点
    CREATE_RESULT make_result = COMPLETE; //生成管理フラグ

    double scale  = 1.0f;     //拡大率
    Pos zoom_piv  = Pos(0,0); //拡大支点
    Pos translate = Pos(0,0); //平行移動量

    bool release_flag  = false;  //クリックトグル化フラグ
    bool move_flag     = false;  //移動フラグ
    bool shift_pressed = false;  //シフトボタンフラグ
    bool ctrl_pressed  = false;  //コントロールフラグ

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

    void Escape();//作成破棄

    void AddEdge     (CEdge* obj);      //オブジェクト追加
    void RemoveObject(CObject* obj);      //オブジェクト削除
    void RemoveEdge  (CEdge* obj);      //オブジェクト削除
    void CompleteObject(CObject* make_obj); //非端点を追加
    //double GetScale()const;                 //スケール取得
    //Pos    GetTranslate()const;             //平行移動量取得

    void Zoom     (double scale,Pos local_piv); //ズーム
    void Translate(Pos local_diff);             //平行移動

    Pos ConvertLocalPos(Pos pos)const; //仮想座標へ変換
    Pos ConvertWorldPos(Pos pos)const; //表示座標へ変換

    void MergePoints();//幾何拘束:マージ

signals:
    void ToggleConflict(bool conflict); //競合シグナル変更
    void ScaleChanged(double value);    //拡大倍率変更
    void RequireRefreshUI();             //UI更新
    void MouseMoved(Pos pos);            //マウス移動



public slots:
    CObject* getHanged(); //直下オブジェクト選定
    void SetScale(double scale);  //スケールセット
    void SetTranslate(Pos trans); //並行移動セット

    void MakeObject();
    CREATE_RESULT MakeJoint(CObject* obj);           //ジョイント作成

    void MakeRestraint(RestraintType type); //拘束作成
    void MakeSmartDimension();              //寸法設定
    bool MakeBlock();                       //物体生成
    void RefreshRestraints();               //拘束更新

    void ImportObjectList(QTreeWidget *list); //オブジェクトリスト選択
    void ExportObjectList (QTreeWidget *list); //オブジェクトリスト描画
    void ImportCBoxList  (QListWidget *list); //ブロックリスト選択
    void ExportCBoxList   (QListWidget *list); //ブロックリスト描画
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
