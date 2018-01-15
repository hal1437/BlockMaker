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
        CObject* ptr = obj->Clone();
        if(ptr != nullptr)translated.push_back(ptr);
    }

    Pos value = Pos(this->ui->XSpinBox->value(),
                    this->ui->YSpinBox->value(),
                    this->ui->ZSpinBox->value());

    if(this->GetTransformMethod() == ABSOLUTE) this->AbsoluteMove(translated.begin(),translated.end(),value);
    else                                       this->RelativeMove(translated.begin(),translated.end(),value);
    emit RepaintRequest();
}

template <class Iterator> QList<CPoint*> MoveTransformDialog::ConvertChildPoint(Iterator begin,Iterator end)const{
    QList<CPoint*> ans;
    for(;begin != end;begin++){
        for(CPoint* child:(*begin)->GetAllChildren()){
            ans.push_back(child);
        }
    }
    unique(ans); //排他
    return ans;
}
MoveTransformDialog::TRANSFORM_METHOD MoveTransformDialog::GetTransformMethod()const{
    if(this->ui->RelativeRadio->isChecked())return RELATIVE;
    else return ABSOLUTE;
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

template <class Iterator> void MoveTransformDialog::AbsoluteMove(Iterator begin,Iterator end, Pos pos){
    //選択された点の平均値を絶対移動
    QList<CPoint*> points = this->ConvertChildPoint(begin,end);

    //中心点を取得
    Pos delta;
    for(CPoint* p :points)delta += *p;
    delta /= points.size();

    //移動
    Pause(begin,end);//更新停止
    for(CPoint* p :points){
        p->MoveAbsolute(*p - delta + pos);
    }
    Restart(begin,end);//更新再開
}
template <class Iterator> void MoveTransformDialog::RelativeMove(Iterator begin,Iterator end, Pos diff){
    //選択された点を相対移動
    Pause(begin,end);//更新停止
    for(CPoint* p :this->ConvertChildPoint(begin,end)){
        p->MoveRelative(diff);
    }
    Restart(begin,end);//更新再開
}

template <class Iterator>
void MoveTransformDialog::Pause(Iterator begin,Iterator end){
    for(;begin != end;begin++){
        (*begin)->ObservePause();
    }
}
template <class Iterator>
void MoveTransformDialog::Restart(Iterator begin,Iterator end){
    for(;begin != end;begin++){
        (*begin)->ObserveRestart();
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
    if(this->GetTransformMethod() == ABSOLUTE) this->AbsoluteMove(this->model->GetSelected().begin(),this->model->GetSelected().end(),value);
    else                                       this->RelativeMove(this->model->GetSelected().begin(),this->model->GetSelected().end(),value);

    this->model->UpdateAnyObjectEmittor();
    RefreshTranslated();
}
void MoveTransformDialog::Duplicate(){

    //値
    Pos value = Pos(this->ui->XSpinBox->value(),
                    this->ui->YSpinBox->value(),
                    this->ui->ZSpinBox->value());

    QList<CObject*> pp;
    this->model->ObservePause();//監視停止

    for(CObject* s : this->model->GetSelected()){
        CObject* dup = s->Clone();   //複製
        if(dup != nullptr){
            pp.push_back(dup);
        }
    }
    this->model->ObserveRestart();//監視再開

    for(CObject* s:pp)this->model->AddObject(s); //モデルに追加

    //移動
    if(this->GetTransformMethod() == ABSOLUTE)this->AbsoluteMove(pp.begin(),pp.end(),value);
    else                                      this->RelativeMove(pp.begin(),pp.end(),value);

    //this->model->AutoMerge();
    this->model->UpdateAnyObjectEmittor();
}

void MoveTransformDialog::Closed(){
    //予測表示を停止
    this->translated.clear();
    emit RepaintRequest();
}



