#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QListWidget>
#include <limits>
#include "SmartDimensionDialog.h"
#include "CBoxDefineDialog.h"
#include "MoveTransformDialog.h"
#include "ExportDialog.h"
#include "CadEditForm.h"

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

protected:
    void keyPressEvent  (QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

signals:
    void ToggleChanged(CEnum state);

public slots:
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
    void ResetAllExpantion(); //拡大、移動リセット
    void MoveTransform();     //移動

    void MakeRestraint();   //拘束作成
    void MakeBlock();       //ブロック作成


    void ReciveObjectListChanged(); //オブジェクトリスト更新
    void ReciveBlockListChanged (); //ブロックリスト更新
    void RefreshStatusBar(Pos);        //ステータスバー更新
};

#endif // MAINWINDOW_H
