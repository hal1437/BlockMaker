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

QVector<CPoint*> MoveTransformDialog::GetSelectedPoint(){
    QVector<CPoint*> ans;
    QVector<CObject*> select = this->model->GetSelected();
    for(CObject* s: select){
        for(CPoint* p:s->GetAllNodes()){
            ans.push_back(p);
        }
    }

    //排他
    std::sort(ans.begin(),ans.end());
    ans.erase(std::unique(ans.begin(),ans.end()),ans.end());

    return ans;
}

void MoveTransformDialog::AbsoluteMove(Pos pos){
    QVector<CPoint*> pp = this->GetSelectedPoint();
    for(CPoint* p :pp)*p = pos;
}

void MoveTransformDialog::RelativeMove(Pos diff){
    QVector<CPoint*> pp = this->GetSelectedPoint();
    for(CPoint* p :pp)*p += diff;
}


void MoveTransformDialog::Accept(){
    Pos value = Pos(this->ui->XSpinBox->value(),
                    this->ui->YSpinBox->value(),
                    this->ui->ZSpinBox->value());

    if(this->ui->RelativeRadio->isChecked()){
        RelativeMove(value);
    }
    if(this->ui->AbsoluteRadio->isChecked()){
        AbsoluteMove(value);
    }
    for(CBlock* block:this->model->GetBlocks()){
        block->RefreshDividePoint();
    }
    emit RepaintRequest();
}

