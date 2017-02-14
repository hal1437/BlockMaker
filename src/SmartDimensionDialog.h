#ifndef SMARTDIMENSIONDIALOG_H
#define SMARTDIMENSIONDIALOG_H

#include <QDialog>

namespace Ui {
class SmartDimensionDialog;
}

class SmartDimensionDialog : public QDialog
{
    Q_OBJECT
    static const int MINIMUM_VALUE = 0;
    static const int MAXIMUM_VALUE = 100000;
    static const int DECIMAL_VALUE = 8;
public:


    double GetValue()const;
    void   SetValue(double value);

    //ラジオの入出力(0:Liner,1:X,2:Y)
    void UseRadioLayout(bool enable);
    int GetCurrentRadio()const;

    explicit SmartDimensionDialog(QWidget *parent = 0);
    ~SmartDimensionDialog();

private:
    Ui::SmartDimensionDialog *ui;
};

#endif // SMARTDIMENSIONDIALOG_H
