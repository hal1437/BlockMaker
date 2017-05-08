#ifndef GRIDFILTERDIALOG_H
#define GRIDFILTERDIALOG_H

#include <QDialog>

namespace Ui {
class GridFilterDialog;
}

class GridFilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GridFilterDialog(QWidget *parent = 0);
    ~GridFilterDialog();

private:
    Ui::GridFilterDialog *ui;

public slots:
    void ToggleRadio();
    void Apply();

signals:
    void ChangeGridStatus(double x,double y);
};

#endif // GRIDFILTERDIALOG_H
