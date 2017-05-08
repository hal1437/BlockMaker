#include "GridFilterDialog.h"
#include "ui_GridFilterDialog.h"

GridFilterDialog::GridFilterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GridFilterDialog)
{
    ui->setupUi(this);
    connect(this->ui->CloseButton ,SIGNAL(pressed()),this,SLOT(close()));
    connect(this->ui->enableRadio ,SIGNAL(pressed()),this,SLOT(ToggleRadio()));
    connect(this->ui->disableRadio,SIGNAL(pressed()),this,SLOT(ToggleRadio()));
    connect(this->ui->ApplyButton ,SIGNAL(pressed()),this,SLOT(Apply()));
}

GridFilterDialog::~GridFilterDialog()
{
    delete ui;
}


void GridFilterDialog::ToggleRadio(){
    if(this->ui->enableRadio->isChecked()){
        this->ui->XSpin->setEnabled(false);
        this->ui->YSpin->setEnabled(false);
        emit ChangeGridStatus(0,0);
    }else{
        this->ui->XSpin->setEnabled(true);
        this->ui->YSpin->setEnabled(true);
        emit ChangeGridStatus(this->ui->XSpin->value(),this->ui->YSpin->value());
    }
}
void GridFilterDialog::Apply(){
    emit ChangeGridStatus(this->ui->XSpin->value(),this->ui->YSpin->value());
}

