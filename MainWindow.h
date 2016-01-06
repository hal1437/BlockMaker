#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include "SmartDimensionDialog.h"
#include "CPoint.h"
#include "CLine.h"
#include "CSpline.h"

namespace Ui {
class MainWindow;
}

enum CEnum{
    Edit,
    Dot,
    Line,
    Arc,
    Spline,
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void mousePressEvent  (QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent * e);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

private:
    Ui::MainWindow *ui;
    CEnum state = Edit;
    CObject* make_obj = nullptr;
    int creating_count=0;
    bool release_flag = false;
    bool move_flag    = false;
    QVector<CObject*> log;

    bool shift_pressed = false;
    bool ctrl_pressed  = false;

public slots:
    void CtrlZ();
    void Delete();
    void Escape();
    void MovedMouse(QMouseEvent* event,CObject* under_object);
    void ConnectSignals();
    void DisconnectSignals();
    void ClearButton();
    void ToggledDot   (bool checked);
    void ToggledLine  (bool checked);
    void ToggledArc   (bool checked);
    void ToggledSpline(bool checked);

    void MakeObject();
    bool MakeJoint(CObject *obj);

};

#endif // MAINWINDOW_H
