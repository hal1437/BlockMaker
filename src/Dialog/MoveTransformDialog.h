#ifndef MOVETRANSFORMDIALOG_H
#define MOVETRANSFORMDIALOG_H

#include <QDialog>
#include "CObject/CObject.h"
#include "CObject/CPoint.h"

namespace Ui {
class MoveTransformDialog;
}

class MoveTransformDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::MoveTransformDialog *ui;
    QVector<CObject*> objects;

public:
    void setObjects(QVector<CObject*> obj);

    explicit MoveTransformDialog(QWidget *parent = 0);
    ~MoveTransformDialog();

public slots:
    void Accept();

signals:
    void RepaintRequest();

};

#endif // MOVETRANSFORMDIALOG_H
