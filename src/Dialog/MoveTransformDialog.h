#ifndef MOVETRANSFORMDIALOG_H
#define MOVETRANSFORMDIALOG_H

#include <QDialog>
#include <QKeyEvent>
#include "CadModelCore.h"

namespace Ui {
class MoveTransformDialog;
}

class MoveTransformDialog : public QDialog,public CadModelCoreInterface
{
    Q_OBJECT

private:
    Ui::MoveTransformDialog *ui;
    QVector<CPoint*> GetSelectedPoint();
public:

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    explicit MoveTransformDialog(QWidget *parent = 0);
    ~MoveTransformDialog();

public slots:
    void PauseChanged();
    void RestartChanged();

    void AbsoluteMove(Pos pos);
    void RelativeMove(Pos diff);
    void Accept();
    void Duplicate();
//    void UpdateObjects(); //オブジェクト更新

signals:
    void RepaintRequest();

};

#endif // MOVETRANSFORMDIALOG_H
