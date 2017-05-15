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


void MoveTransformDialog::setObjects(QVector<CObject*> obj){
    this->objects = obj;
}

void MoveTransformDialog::Accept(){
    if(this->ui->RelativeRadio->isChecked()){
        for(CObject* obj : this->objects){
            obj->Move(Pos(this->ui->XSpinBox->value(),this->ui->YSpinBox->value()));
        }
    }
    if(this->ui->AbsoluteRadio->isChecked()){
        for(CObject* obj : this->objects){
            Pos p(0,0);
            if(obj->is<CPoint>())p = *dynamic_cast<CPoint*>(obj);
            obj->Move(Pos(this->ui->XSpinBox->value(),this->ui->YSpinBox->value()) - p);
        }
    }
    emit RepaintRequest();
}

