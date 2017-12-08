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
    connect(this->ui->XSpinBox     ,SIGNAL(valueChanged(double)) ,this,SLOT(ValueChangedEmitter(double)));
    connect(this->ui->YSpinBox     ,SIGNAL(valueChanged(double)) ,this,SLOT(ValueChangedEmitter(double)));
    connect(this->ui->ZSpinBox     ,SIGNAL(valueChanged(double)) ,this,SLOT(ValueChangedEmitter(double)));
    connect(this->ui->AbsoluteRadio,SIGNAL(clicked(bool)) ,this,SLOT(RadioChangedEmitter(bool)));
    connect(this->ui->RelativeRadio,SIGNAL(clicked(bool)) ,this,SLOT(RadioChangedEmitter(bool)));
    connect(this              ,SIGNAL(rejected()),this,SLOT(Closed()));
    this->setWindowTitle("MoveTransform");
    this->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
}
MoveTransformDialog::~MoveTransformDialog()
{
    delete ui;
}

void MoveTransformDialog::SetModel(CadModelCore *m){
    this->model = m;
    connect(this->model,SIGNAL(UpdateSelected()),this,SLOT(RefreshTranslated()));
}
void MoveTransformDialog::RefreshTranslated(){
    translated.clear();
    //複製
    for(CObject* obj:this->model->GetSelected()){
        translated.push_back(obj->Clone());
    }

    Pos value = Pos(this->ui->XSpinBox->value(),
                    this->ui->YSpinBox->value(),
                    this->ui->ZSpinBox->value());
    if(this->GetTransformMethod() == ABSOLUTE) this->AbsoluteMove(translated,value);
    else                                       this->RelativeMove(translated,value);
    emit RepaintRequest();
}

QVector<CPoint*> MoveTransformDialog::ConvertChildPoint(QVector<CObject *> objects)const{
    QVector<CPoint*> ans;
    for(CObject* obj:objects){
        for(CPoint* child:obj->GetAllChildren()){
            ans.push_back(child);
        }
    }
    unique(ans);//排他
    return ans;
}
MoveTransformDialog::TRANSFORM_METHOD MoveTransformDialog::GetTransformMethod()const{
    if(this->ui->RelativeRadio->isChecked())return RELATIVE;
    if(this->ui->AbsoluteRadio->isChecked())return ABSOLUTE;
    // ????
}

void MoveTransformDialog::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Shift){
        this->ui->XSpinBox->setValue(-this->ui->XSpinBox->value());
        this->ui->YSpinBox->setValue(-this->ui->YSpinBox->value());
        this->ui->ZSpinBox->setValue(-this->ui->ZSpinBox->value());
    }
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        this->Accept();
    }
}
void MoveTransformDialog::keyReleaseEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Shift){
        this->ui->XSpinBox->setValue(-this->ui->XSpinBox->value());
        this->ui->YSpinBox->setValue(-this->ui->YSpinBox->value());
        this->ui->ZSpinBox->setValue(-this->ui->ZSpinBox->value());
    }
}

void MoveTransformDialog::AbsoluteMove(QVector<CObject *> objects, Pos pos){
    //選択された点の平均値を絶対移動
    QVector<CPoint*> points =  this->ConvertChildPoint(objects);

    //中心点を取得
    Pos delta;
    for(CPoint* p :points)delta += *p;
    delta /= points.size();

    //移動
    PauseChanged(objects);//更新停止
    for(CPoint* p :points){
        p->MoveAbsolute(*p - delta + pos);
    }
    RestartChanged(objects);//更新再開
}
void MoveTransformDialog::RelativeMove(QVector<CObject *> objects, Pos diff){
    //選択された点を相対移動
    PauseChanged(objects);//更新停止
    for(CPoint* p :this->ConvertChildPoint(objects)){
        p->MoveRelative(diff);
    }
    RestartChanged(objects);//更新再開
}

void MoveTransformDialog::PauseChanged(QVector<CObject *> objects){
    for(CObject* obj: objects){
        obj->ObservePause();
    }
}
void MoveTransformDialog::RestartChanged(QVector<CObject *> objects){
    for(CObject* obj:objects){
        obj->ObserveRestart();
    }
}
void MoveTransformDialog::DrawTranslated(Pos camera,Pos center){
    //色を保存
    float old_color[4];
    glGetFloatv  (GL_CURRENT_COLOR,old_color);

    glColor3f(1,1,0);
    for(CObject* obj:this->translated){
        obj->DrawGL(camera,center);
    }
    //色を復元
    glColor4f(old_color[0],old_color[1],old_color[2], old_color[3]);
}
void MoveTransformDialog::ValueChangedEmitter(double){
    RefreshTranslated();
}
void MoveTransformDialog::RadioChangedEmitter(bool){
    RefreshTranslated();
}

void MoveTransformDialog::Accept(){
    //値
    Pos value = Pos(this->ui->XSpinBox->value(),
                    this->ui->YSpinBox->value(),
                    this->ui->ZSpinBox->value());
    if(this->GetTransformMethod() == ABSOLUTE) this->AbsoluteMove(this->model->GetSelected(),value);
    else                                       this->RelativeMove(this->model->GetSelected(),value);
    /*
    for(CBlock* block:this->model->GetBlocks()){
        //block->RefreshDividePoint();
    }*/
    this->model->UpdateAnyObjectEmittor();
    RefreshTranslated();
}
void MoveTransformDialog::Duplicate(){
    //値
    Pos value = Pos(this->ui->XSpinBox->value(),
                    this->ui->YSpinBox->value(),
                    this->ui->ZSpinBox->value());

    QVector<CObject*> pp;
    for(CObject* s : this->model->GetSelected()){
        CObject* dup = s->Clone();   //複製
        this->model->AutoMerge(dup); //オートマージ
        this->model->AddObject(dup); //モデルに追加
        for(CPoint* pos:dup->GetAllChildren()){
            pp.push_back(pos);
        }
    }
    //重複削除
    unique(pp);

    //移動
    if(this->GetTransformMethod() == ABSOLUTE)this->AbsoluteMove(pp,value);
    else                                      this->RelativeMove(pp,value);

    this->model->UpdateAnyObjectEmittor();
    this->model->AutoMerge();
}

void MoveTransformDialog::Closed(){
    //予測表示を停止
    this->translated.clear();
    emit RepaintRequest();
}



