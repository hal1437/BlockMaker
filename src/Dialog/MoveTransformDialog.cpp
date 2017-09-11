#include "MoveTransformDialog.h"
#include "ui_MoveTransformDialog.h"

MoveTransformDialog::MoveTransformDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MoveTransformDialog)
{
    ui->setupUi(this);
    connect(this->ui->ApplyButton    ,SIGNAL(clicked()),this,SLOT(Accept()));
    connect(this->ui->DuplicateButton,SIGNAL(clicked()),this,SLOT(Duplicate()));
    connect(this->ui->CloseButton    ,SIGNAL(clicked()),this,SLOT(close()));
    this->setWindowTitle("MoveTransform");
    this->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
}

MoveTransformDialog::~MoveTransformDialog()
{
    delete ui;
}

QVector<CPoint*> MoveTransformDialog::GetSelectedPoint(){
    QVector<CPoint*> ans;
    for(CObject* obj:this->model->GetSelected()){
        for(CPoint* child:obj->GetAllChildren()){
            ans.push_back(child);
        }
    }
    unique(ans);//排他
    return ans;
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

void MoveTransformDialog::AbsoluteMove(Pos pos){
    //選択された点の平均値を絶対移動
    Pos delta;
    QVector<CPoint*> array =  this->GetSelectedPoint();
    for(CPoint* p :array)delta += *p;
    delta /= array.size();

    //移動
    for(CPoint* p :array){
        if(this->ui->MovingCombo->currentText() == "追従")p->MoveAbsolute(*p-delta+pos);
        if(this->ui->MovingCombo->currentText() == "強制")*p = *p-delta+pos;
    }
}

void MoveTransformDialog::RelativeMove(Pos diff){
    //選択された点を相対移動
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
        //block->RefreshDividePoint();
    }
    emit RepaintRequest();
}
void MoveTransformDialog::Duplicate(){
    Pos value = Pos(this->ui->XSpinBox->value(),
                    this->ui->YSpinBox->value(),
                    this->ui->ZSpinBox->value());
    QVector<CPoint*> pp;
    for(CObject* s : this->model->GetSelected()){
        CObject* dup = s->Clone();
        this->model->AutoMerge(dup);
        this->model->AddObject(dup);
        for(CPoint* pos:dup->GetAllChildren()){
            pp.push_back(pos);
        }
    }
    //重複削除
    unique(pp);

    //移動
    for(CPoint* pos:pp){
        if(this->ui->RelativeRadio->isChecked()){
            if(this->ui->MovingCombo->currentText() == "追従")pos->MoveRelative(value);
            if(this->ui->MovingCombo->currentText() == "強制")*pos += value;
        }
        if(this->ui->AbsoluteRadio->isChecked()){
            if(this->ui->MovingCombo->currentText() == "追従")pos->MoveAbsolute(value);
            if(this->ui->MovingCombo->currentText() == "強制")*pos = value;
        }
    }



    this->model->AutoMerge();
}


