#ifndef MOVETRANSFORMDIALOG_H
#define MOVETRANSFORMDIALOG_H

#include <QDialog>
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

    explicit MoveTransformDialog(QWidget *parent = 0);
    ~MoveTransformDialog();

public slots:
    void AbsoluteMove(Pos pos);
    void RelativeMove(Pos diff);
    void Accept();
    void Duplicate();

signals:
    void RepaintRequest();

};

#endif // MOVETRANSFORMDIALOG_H
