#include "MoveTransformDialog.h"
#include "ui_MoveTransformDialog.h"

MoveTransformDialog::MoveTransformDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MoveTransformDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("MoveTransformDialog");
    connect(this->ui->ApplyButton,SIGNAL(clicked()),this,SLOT(Accept()));
    connect(this->ui->CloseButton ,SIGNAL(clicked()),this,SLOT(close()));
}

MoveTransformDialog::~MoveTransformDialog()
{
    delete ui;
}


void MoveTransformDialog::Accept(){
    Pos value = Pos(this->ui->XSpinBox->value(),
                    this->ui->YSpinBox->value(),
                    this->ui->ZSpinBox->value());

    if(this->ui->RelativeRadio->isChecked()){
        for(CObject* obj : this->model->GetSelected()){
            obj->Move(value);
        }
    }
    if(this->ui->AbsoluteRadio->isChecked()){
        for(CObject* obj : this->model->GetSelected()){
            Pos p = Pos(0,0);
            if(obj->is<CPoint>())p = *dynamic_cast<CPoint*>(obj);
            obj->Move(value - p);
        }
    }
    for(CBlock* block:this->model->GetBlocks()){
        block->RefreshDividePoint();
    }
    emit RepaintRequest();
}

