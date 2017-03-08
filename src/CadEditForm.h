#ifndef CADEDITFORM_H
#define CADEDITFORM_H

#include <QWidget>
#include <QDebug>
#include <QMouseEvent>
#include <QListWidget>
#include <QListWidgetItem>
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

namespace Ui {
class CadEditForm;
}

class CadEditForm : public QWidget
{
    Q_OBJECT
private:
    QVector<CObject*>         objects;    //物体
    QVector<SmartDimension*>  dimensions; //寸法
    QVector<Restraint*>       restraints; //拘束
    QVector<CBlock>          blocks;     //物体
    QVector<CBlock*>         selecting_block; //選択物体

    double scale = 1.0f;
    Pos translate = Pos(0,0);
public:

    void AddObject(CObject* obj);           //物体追加
    void RemoveObject(CObject* obj);        //物体削除
    void CompleteObject(CObject* make_obj); //非端点を追加
    double GetScale()const;     //スケール
    Pos    GetTranslate()const; //平行移動

    void paintEvent    (QPaintEvent*  event); //描画イベントハンドラ
    void mouseMoveEvent(QMouseEvent*  event); //マウス移動
    void resizeEvent   (QResizeEvent* event); //リサイズ

    Pos ConvertLocalPos(Pos pos)const; //仮装座標へ変換
    Pos ConvertWorldPos(Pos pos)const; //表示座標へ変換

    void Zoom(double scale,Pos local_piv); //ズーム
    void Move(Pos local_diff);             //平行移動


    explicit CadEditForm(QWidget *parent = 0);
    ~CadEditForm();

private:
    Ui::CadEditForm *ui;
    Pos zoom_piv; //ズーム支点

signals:
    void MovedMouse(QMouseEvent *event, CObject *under_object);
    void ToggleConflict(bool conflict);

public slots:
    CObject* getHanged(); //直下オブジェクト選定
    void SetScale(double scale);  //スケールセット
    void SetTranslate(Pos trans); //並行移動セット
    void MakeRestraint(RestraintType type); //拘束作成
    void MakeSmartDimension();              //寸法設定
    bool MakeBlock();                       //物体生成
    void RefreshRestraints();               //拘束更新
    void ApplyObjectList(QListWidget *list); //オブジェクトリスト選択
    void DrawObjectList (QListWidget *list); //オブジェクトリスト描画
    void ApplyCBoxList  (QListWidget *list); //ブロックリスト選択
    void DrawCBoxList   (QListWidget *list); //ブロックリスト描画
    void ConfigureBlock (QListWidgetItem* item); //ブロック再編集

    void ResetAllExpantion();//拡大、移動リセット

};

#endif // CADEDITFORM_H
