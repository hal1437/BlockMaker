#include "PropertyDefinitionDialog.h"
#include "ui_PropertyDefinitionDialog.h"

//CObjectの配列ARRAY内の特定のメンバが全て等しいかどうか判定
#define SELECTED_SAME_VALUE(ARRAY,TYPE,MEMBER) \
std::count_if(ARRAY.begin(),ARRAY.end(),[&](CObject* obj){\
    return (dynamic_cast<TYPE>(obj)->get##MEMBER() == dynamic_cast<TYPE>(ARRAY.first())->get##MEMBER());\
}) == ARRAY.size()


void PropertyDefinitionDialog::ConstructFace(){
    //Face用レイアウト構築
    this->constructed = CONSTRUCTED::FACE;
    this->face_boundary_label .show();
    this->face_boundary_combo .show();
    this->face_boundary_button.show();
    QVector<CObject*> selected = this->model->GetSelected();

    //全ての境界条件が同じであれば
    if(SELECTED_SAME_VALUE(selected,CFace*,Boundary)){
        this->face_boundary_combo.setCurrentIndex(static_cast<int>(dynamic_cast<CFace*>(selected.first())->getBoundary().type));
    }
}

void PropertyDefinitionDialog::ConstructEdge(){
    //Edge用レイアウト構築
    this->constructed = CONSTRUCTED::EDGE;
    this->edge_divide_label.show();
    this->edge_divide_spin.show();
    this->edge_grading_label.show();
    this->edge_grading_spin.show();

    //全て分割数が同じであれば
    QVector<CObject*> selected = this->model->GetSelected();
    if(SELECTED_SAME_VALUE(selected,CEdge*,Divide)){
        this->edge_divide_spin.setValue(dynamic_cast<CEdge*>(selected.first())->getDivide());
    }
    //全てのメッシュ寄せ係数が同じであれば
    if(SELECTED_SAME_VALUE(selected,CEdge*,Grading)){
        this->edge_grading_spin.setValue(dynamic_cast<CEdge*>(selected.first())->getGrading());
    }
}
bool PropertyDefinitionDialog::CheckAvailable()const{
    QVector<CObject*> selected = this->model->GetSelected();
    if(selected.size() > 0 && std::all_of(selected.begin(),selected.end(),[](CObject* obj){return obj->is<CEdge>();}))return true;
    if(selected.size() > 0 && std::all_of(selected.begin(),selected.end(),[](CObject* obj){return obj->is<CFace>();}))return true;
    return false;
}

void PropertyDefinitionDialog::SetModel(CadModelCore *m){
    this->model = m;
    connect(this->model,SIGNAL(UpdateSelected()),this,SLOT(UpdateLayout()));
    UpdateLayout();
}

PropertyDefinitionDialog::PropertyDefinitionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PropertyDefinitionDialog)
{
    ui->setupUi(this);
    connect(this->ui->ApplyButton,SIGNAL(pressed()),this,SLOT(Accept()));
    connect(this->ui->CloseButton,SIGNAL(pressed()),this,SLOT(close()));
    connect(&face_boundary_button,SIGNAL(pressed()),this,SLOT(ShowBoundayDefinitionDialog()));
    this->setWindowTitle("Property");
    this->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);

    //初期値設定
    this->name_label          .setText("オブジェクト名");
    this->face_boundary_label .setText("境界タイプ");
    this->face_boundary_button.setText("...");
    this->edge_divide_label   .setText("分割数");
    this->edge_grading_label  .setText("エッジ寄せ係数");
    this->edge_divide_spin .setValue(0);
    this->edge_divide_spin .setMaximum(9999);
    this->edge_divide_spin .setMinimum(0);
    this->edge_grading_spin.setValue(0);
    this->edge_grading_spin.setMaximum(10000);
    this->edge_grading_spin.setMinimum(0);
    this->edge_grading_spin.setSingleStep(0.01);
    this->edge_grading_spin.setDecimals(6);

    //コンボボックス指定
    for(Boundary boundary: BoundaryDefinitionDialog::boundary_list){
        this->face_boundary_combo.addItem(boundary.name + " <" + Boundary::BoundaryTypeToString(boundary.type) + ">");
    }

    this->ui->Grid->addWidget(&this->name_label          ,0,0);
    this->ui->Grid->addWidget(&this->name_edit           ,0,1);
    this->ui->Grid->addWidget(&this->face_boundary_label ,1,0);
    this->ui->Grid->addWidget(&this->face_boundary_combo ,1,1);
    this->ui->Grid->addWidget(&this->face_boundary_button,1,2);
    this->ui->Grid->addWidget(&this->edge_divide_label   ,2,0);
    this->ui->Grid->addWidget(&this->edge_divide_spin    ,2,1);
    this->ui->Grid->addWidget(&this->edge_grading_label  ,3,0);
    this->ui->Grid->addWidget(&this->edge_grading_spin   ,3,1);
}
PropertyDefinitionDialog::~PropertyDefinitionDialog()
{
    delete ui;
}

void PropertyDefinitionDialog::UpdateLayout(){
    //レイアウト解除
    this->name_label          .hide();
    this->name_edit           .hide();
    this->face_boundary_label .hide();
    this->face_boundary_combo .hide();
    this->face_boundary_button.hide();
    this->edge_divide_label   .hide();
    this->edge_divide_spin    .hide();
    this->edge_grading_label  .hide();
    this->edge_grading_spin   .hide();

    //表示タイプ選定
    QVector<CObject*> selected = this->model->GetSelected();
    if     (selected.size() > 0 && std::all_of(selected.begin(),selected.end(),[](CObject* obj){return obj->is<CEdge>();}))this->ConstructEdge();
    else if(selected.size() > 0 && std::all_of(selected.begin(),selected.end(),[](CObject* obj){return obj->is<CFace>();}))this->ConstructFace();
    else this->constructed = CONSTRUCTED::EMPTY;

    if(selected.size() > 0){
        //全て名前が同じであれば
        if(SELECTED_SAME_VALUE(selected,CObject*,Name)){
            this->name_edit.setText(selected.first()->getName());
        }else{
            this->name_edit.clear();
        }
        this->name_label.show();
        this->name_edit .show();

        //変更不可なものが含まれていれば
        if(exist(selected,CPoint::origin) ||
           exist(selected,CFace::base[0]) ||
           exist(selected,CFace::base[1]) ||
           exist(selected,CFace::base[2])){
            this->constructed = CONSTRUCTED::EMPTY;
            this->name_label.setEnabled(false);
            this->name_edit .setEnabled(false);
        }else{
            this->name_label.setEnabled(true);
            this->name_edit .setEnabled(true);
        }
    }
    this->resize(this->sizeHint());
    this->repaint();
}

void PropertyDefinitionDialog::ShowBoundayDefinitionDialog(){
    BoundaryDefinitionDialog* diag = new BoundaryDefinitionDialog();
    diag->SetModel(model);
    diag->exec();

    //コンボ状態保存
    QString save = this->face_boundary_combo.currentText();

    //コンボ更新
    this->face_boundary_combo.clear();
    for(Boundary boundary: BoundaryDefinitionDialog::boundary_list){
        this->face_boundary_combo.addItem(boundary.name + " <" + Boundary::BoundaryTypeToString(boundary.type) + ">");
    }
    //コンボ状態復元
    this->face_boundary_combo.setCurrentText(save);
}

void PropertyDefinitionDialog::Accept(){
    //適用処理
    QVector<CObject*> selected = this->model->GetSelected();

    //更新停止対象選定
    QVector<CObject*> paused;
    if(this->constructed == CONSTRUCTED::FACE){
        for(CBlock* block: this->model->GetBlocks()){
            for(CObject* obj: selected){
                for(int i=0;i<block->GetChildCount();i++){
                    if(block->GetChild(i) == obj){
                        paused.push_back(block);
                    }
                }
            }
        }
    }
    if(this->constructed == CONSTRUCTED::EDGE){
        for(CFace* face: this->model->GetFaces()){
            for(CObject* obj: selected){
                for(int i=0;i<face->GetChildCount();i++){
                    if(face->GetChild(i) == obj){
                        paused.push_back(face);
                    }
                }
            }
        }
    }
    //更新停止
    for(CObject* obj:paused) obj->ObservePause();

    //値を代入
    for(CObject* obj: selected){
        //名前
        if(this->name_edit.text() != "")obj->setName(this->name_edit.text());

        //面
        if(this->constructed == CONSTRUCTED::FACE){
            //境界タイプ
            if(this->face_boundary_combo.currentText() != ""){

                int index = IndexOf_if(BoundaryDefinitionDialog::boundary_list,[&](Boundary b){
                    return (b.name + " <" + Boundary::BoundaryTypeToString(b.type) + ">") == face_boundary_combo.currentText();
                });
                if(index != -1){
                    dynamic_cast<CFace*>(obj)->setBoundary(BoundaryDefinitionDialog::boundary_list[index]);
                }
            }
        }
        //線
        if(this->constructed == CONSTRUCTED::EDGE){
            //分割数
            if(this->edge_divide_spin .value() != 0)dynamic_cast<CEdge*>(obj)->setDivide (this->edge_divide_spin.value());
            //メッシュ寄せ係数
            if(this->edge_grading_spin.value() != 0)dynamic_cast<CEdge*>(obj)->setGrading(this->edge_grading_spin.value());
        }
    }

    //更新再開
    for(CObject* obj:paused) obj->ObserveRestart();

    emit RepaintRequest();
}

