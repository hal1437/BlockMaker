#include "SmartDimensionDialog.h"
#include "ui_SmartDimensionDialog.h"


double SmartDimensionDialog::GetValue()const{
    return ui->lineEdit->text().toDouble();
}
void   SmartDimensionDialog::SetValue(double value){
    ui->lineEdit->setText(QString::number(value));
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
