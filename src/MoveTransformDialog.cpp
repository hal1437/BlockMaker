#include "MoveTransformDialog.h"
#include "ui_MoveTransformDialog.h"

MoveTransformDialog::MoveTransformDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MoveTransformDialog)
{
    ui->setupUi(this);
    connect(this->ui->buttonBox,SIGNAL(accepted()),this,SLOT(accept()));
}

MoveTransformDialog::~MoveTransformDialog()
{
    delete ui;
}

void MoveTransformDialog::Accept(){

}

