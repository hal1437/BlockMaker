#include "MoveTransformDialog.h"
#include "ui_MoveTransformDialog.h"

MoveTransformDialog::MoveTransformDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MoveTransformDialog)
{
    ui->setupUi(this);
    connect(this->ui->buttonBox,SIGNAL(accepted()),this,SLOT(Accept()));
}

MoveTransformDialog::~MoveTransformDialog()
{
    delete ui;
}

void MoveTransformDialog::Accept(){
    if(this->ui->RelativeRadio->isChecked()){
        for(CObject* obj : CObject::selected){
            obj->Move(Pos(this->ui->XSpinBox->value(),this->ui->YSpinBox->value()));
        }
    }
    if(this->ui->AbsoluteRadio->isChecked()){
        for(CObject* obj : CObject::selected){
            obj->Move(Pos(this->ui->XSpinBox->value(),this->ui->YSpinBox->value()) - obj->GetJointPos(0));
        }
    }
}

