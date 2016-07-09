#include "SmartDimensionDialog.h"
#include "ui_SmartDimensionDialog.h"


double SmartDimensionDialog::GetValue()const{
    return ui->lineEdit->text().toDouble();
}
void   SmartDimensionDialog::SetValue(double value){
    ui->lineEdit->setText(QString::number(value));
}
void SmartDimensionDialog::UseRadioLayout(bool enable){
    ui->LinerRadio->setEnabled(enable);
    ui->XRadio->setEnabled(enable);
    ui->YRadio->setEnabled(enable);
}
int SmartDimensionDialog::GetCurrentRadio()const{
    if(ui->LinerRadio->isChecked())return 0;
    if(ui->XRadio    ->isChecked())return 1;
    if(ui->YRadio    ->isChecked())return 2;
    return -1;
}



SmartDimensionDialog::SmartDimensionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SmartDimensionDialog)
{
    ui->setupUi(this);
    ui->lineEdit->setValidator(new QDoubleValidator(MINIMUM_VALUE, MAXIMUM_VALUE,DECIMAL_VALUE, this));
}

SmartDimensionDialog::~SmartDimensionDialog()
{
    delete ui;
}
