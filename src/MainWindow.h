#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QListWidget>
#include <limits>
#include <random>
#include "CadModelCore.h"
#include "SolidEditForm.h"

#include "Dialog/SmartDimensionDialog.h"
#include "Dialog/CBoxDefineDialog.h"
#include "Dialog/MoveTransformDialog.h"
#include "Dialog/ExportDialog.h"
#include "Dialog/GridFilter.h"
#include "Dialog/GridFilterDialog.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow ,CadModelCoreInterface
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    MoveTransformDialog* move_diag;

protected:
    void keyPressEvent  (QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

signals:
    void ToggleChanged(MAKE_OBJECT state);

public slots:
    void SetModel(CadModelCore* model);

    void CtrlZ();             //1手戻る
    void Delete();            //削除
    void ConnectSignals();    //シグナル接続
    void DisconnectSignals(); //シグナル切断
    void ClearButton();       //全ボタンリセット
    void RefreshUI();         //UI更新

    void ToggledPoint  (bool checked);
    void ToggledLine   (bool checked);
    void ToggledArc    (bool checked);
    //void ToggledRect   (bool checked);
    void ToggledSpline (bool checked);
    void ToggleConflict(bool conflict);

    void ShowMoveTransform();     //移動ダイアログ表示
    void ShowGridFilter();        //グリッドフィルター表示

    void MakeRestraint();   //拘束作成
    void MakeBlock();       //ブロック作成
    void MakeFace();        //面作成

    void RefreshStatusBar(Pos pos);        //ステータスバー更新

    void ShowObjectList();

    void Save();
    void Load();
    void Export();
};

#endif // MAINWINDOW_H
