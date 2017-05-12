#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QListWidget>
#include <limits>
#include "CadModelCore.h"

#include "Dialog/SolidView.h"
#include "Dialog/SmartDimensionDialog.h"
#include "Dialog/CBoxDefineDialog.h"
#include "Dialog/MoveTransformDialog.h"
#include "Dialog/ExportDialog.h"
#include "CadEditForm.h"
#include "Dialog/GridFilter.h"
#include "Dialog/GridFilterDialog.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    CadModelCore* model;

protected:
    void keyPressEvent  (QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

signals:
    void ToggleChanged(CEnum state);

public slots:
    void SetModel(CadModelCore* model);

    void CtrlZ();
    void Delete();
    void ConnectSignals();
    void DisconnectSignals();
    void ClearButton();
    void RefreshUI();
    void ToggledDot    (bool checked);
    void ToggledLine   (bool checked);
    void ToggledArc    (bool checked);
    void ToggledRect   (bool checked);
    void ToggledSpline (bool checked);
    void ToggleConflict(bool conflict);
    void ResetAllExpantion();     //拡大、移動リセット
    void ShowMoveTransform();     //移動
    void ShowSolidView();         //立体表示
    void ShowGridFilter();        //グリッドフィルター

    void MakeRestraint();   //拘束作成
    void MakeBlock();       //ブロック作成


    void ReciveObjectListChanged(); //オブジェクトリスト更新
    void ReciveBlockListChanged (); //ブロックリスト更新
    void RefreshStatusBar(Pos);        //ステータスバー更新
};

#endif // MAINWINDOW_H
