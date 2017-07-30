#include "MoveTransformDialog.h"
#include "ui_MoveTransformDialog.h"

MoveTransformDialog::MoveTransformDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MoveTransformDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("MoveTransformDialog");
    connect(this->ui->ApplyButton    ,SIGNAL(clicked()),this,SLOT(Accept()));
    connect(this->ui->DuplicateButton,SIGNAL(clicked()),this,SLOT(Duplicate()));
    connect(this->ui->CloseButton    ,SIGNAL(clicked()),this,SLOT(close()));
}

MoveTransformDialog::~MoveTransformDialog()
{
    delete ui;
}

void MoveTransformDialog::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Shift){
        this->ui->XSpinBox->setValue(-this->ui->XSpinBox->value());
        this->ui->YSpinBox->setValue(-this->ui->YSpinBox->value());
        this->ui->ZSpinBox->setValue(-this->ui->ZSpinBox->value());
    }
}

void MoveTransformDialog::keyReleaseEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Shift){
        this->ui->XSpinBox->setValue(-this->ui->XSpinBox->value());
        this->ui->YSpinBox->setValue(-this->ui->YSpinBox->value());
        this->ui->ZSpinBox->setValue(-this->ui->ZSpinBox->value());
    }
}


QVector<CPoint*> MoveTransformDialog::GetSelectedPoint(){
    QVector<CPoint*> ans;
    for(CObject* s : this->model->GetSelected()){
        for(CPoint* p : s->GetAllChildren()){
            ans.push_back(p);
        }
    }

    //排他
    unique(ans);
    std::sort(ans.begin(),ans.end());
    ans.erase(std::unique(ans.begin(),ans.end()),ans.end());
    return ans;
}

void MoveTransformDialog::AbsoluteMove(Pos pos){
    for(CPoint* p :this->GetSelectedPoint()){
        if(this->ui->MovingCombo->currentText() == "追従")p->MoveAbsolute(pos);
        if(this->ui->MovingCombo->currentText() == "強制")*p = pos;
    }
}

void MoveTransformDialog::RelativeMove(Pos diff){
    for(CPoint* p :this->GetSelectedPoint()){
        if(this->ui->MovingCombo->currentText() == "追従")p->MoveRelative(diff);
        if(this->ui->MovingCombo->currentText() == "強制")*p += diff;
    }
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
void MoveTransformDialog::Duplicate(){
    Pos value = Pos(this->ui->XSpinBox->value(),
                    this->ui->YSpinBox->value(),
                    this->ui->ZSpinBox->value());
    for(CObject* s : this->model->GetSelected()){
        CObject* dup = s->Clone();
        this->model->AutoMerge(dup);
        this->model->AddObject(dup);
        for(CPoint* pos:dup->GetAllChildren()){
            *pos += value;
        }
    }
    this->model->AutoMerge();
}


