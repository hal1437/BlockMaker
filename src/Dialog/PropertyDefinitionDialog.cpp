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
    QList<CObject*> selected = this->model->GetSelected();

    //コンボ更新
    this->face_boundary_combo.clear();
    for(Boundary boundary: BoundaryDefinitionDialog::boundary_list){
        this->face_boundary_combo.addItem(boundary.name + " <" + Boundary::BoundaryTypeToString(boundary.type) + ">");
    }

    //全ての境界条件が同じであれば
    if(SELECTED_SAME_VALUE(selected,CFace*,Boundary)){
        Boundary boundary = dynamic_cast<CFace*>(selected.first())->getBoundary();
        this->face_boundary_combo.setCurrentText(boundary.name + " <" + Boundary::BoundaryTypeToString(boundary.type) + ">");
    }
    this->resize(250,100);
}

void PropertyDefinitionDialog::ConstructEdge(){
    //Edge用レイアウト構築
    this->constructed = CONSTRUCTED::EDGE;
    this->edge_all_divide_label.show();
    this->edge_all_divide_spin .show();
    this->edge_add_grading_button.show();
    this->edge_remove_grading_button.show();
    this->edge_multi_grading_table.show();

    //テーブル設定
    this->edge_multi_grading_table.clear();
    this->edge_multi_grading_table.setColumnCount(3);
    this->edge_multi_grading_table.setHorizontalHeaderItem(0,new QTableWidgetItem("長さ割合"));
    this->edge_multi_grading_table.setHorizontalHeaderItem(1,new QTableWidgetItem("分割数割合"));
    this->edge_multi_grading_table.setHorizontalHeaderItem(2,new QTableWidgetItem("エッジ寄せ係数"));
    this->edge_multi_grading_table.setColumnWidth(0,80);
    this->edge_multi_grading_table.setColumnWidth(1,80);
    this->edge_multi_grading_table.setColumnWidth(2,120);
    this->edge_dir_spins.clear();
    this->edge_divide_spins.clear();
    this->edge_grading_spins.clear();

    //全て分割数が同じであれば
    QList<CObject*> selected = this->model->GetSelected();
    if(SELECTED_SAME_VALUE(selected,CEdge*,Divide)){
        this->edge_all_divide_spin.setValue(dynamic_cast<CEdge*>(selected.first())->getDivide());
    }
    //全てのメッシュ寄せ係数が同じであれば
    if(SELECTED_SAME_VALUE(selected,CEdge*,Grading)){
        CEdge::Grading g = dynamic_cast<CEdge*>(selected.first())->getGrading();
        //列数設定
        this->edge_multi_grading_table.setRowCount(g.elements.size());
        //セットアップ
        for(int i=0;i<g.elements.size();i++){
            //新規スピンボックス設定
            this->edge_dir_spins    .push_back(new QDoubleSpinBox());
            this->edge_divide_spins .push_back(new QDoubleSpinBox());
            this->edge_grading_spins.push_back(new QDoubleSpinBox());

            //値設定
            this->SetupDoubleSpin(this->edge_dir_spins    .last(),
                                  this->edge_divide_spins .last(),
                                  this->edge_grading_spins.last());
            this->edge_dir_spins    .last()->setValue(g.elements[i].dir);
            this->edge_divide_spins .last()->setValue(g.elements[i].cell);
            this->edge_grading_spins.last()->setValue(g.elements[i].grading);

            //新規設定
            this->edge_multi_grading_table.setCellWidget(i,0,this->edge_dir_spins.last());
            this->edge_multi_grading_table.setCellWidget(i,1,this->edge_divide_spins.last());
            this->edge_multi_grading_table.setCellWidget(i,2,this->edge_grading_spins.last());
        }
    }else{
        //新規スピンボックス設定
        this->edge_dir_spins    .push_back(new QDoubleSpinBox());
        this->edge_divide_spins .push_back(new QDoubleSpinBox());
        this->edge_grading_spins.push_back(new QDoubleSpinBox());

        //値設定
        this->SetupDoubleSpin(this->edge_dir_spins    .last(),
                              this->edge_divide_spins .last(),
                              this->edge_grading_spins.last());
        this->edge_dir_spins    .last()->setValue(1.0);
        this->edge_divide_spins .last()->setValue(1.0);
        this->edge_grading_spins.last()->setValue(1.0);

        //新規設定
        this->edge_multi_grading_table.setCellWidget(0,0,this->edge_dir_spins.last());
        this->edge_multi_grading_table.setCellWidget(0,1,this->edge_divide_spins.last());
        this->edge_multi_grading_table.setCellWidget(0,2,this->edge_grading_spins.last());
    }
    this->resize(350,250 + this->edge_multi_grading_table.rowCount()*this->edge_multi_grading_table.rowHeight(0));
}
void PropertyDefinitionDialog::SetupDoubleSpin(QDoubleSpinBox* dir,QDoubleSpinBox* divide,QDoubleSpinBox* grading)const{
    dir->setDecimals(4);
    dir->setMaximum(1.0e10);
    dir->setMinimum(0.0e-10);
    dir->setSingleStep(0.01);
    divide->setDecimals(4);
    divide->setMaximum(1.0e10);
    divide->setMinimum(0.0e-10);
    divide->setSingleStep(0.01);
    grading->setDecimals(4);
    grading->setMaximum(1.0e10);
    grading->setMinimum(1.0e-10);
    grading->setSingleStep(0.01);
}

bool PropertyDefinitionDialog::CheckAvailable()const{
    QList<CObject*> selected = this->model->GetSelected();
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
    this->name_label             .setText("オブジェクト名");
    this->face_boundary_label    .setText("境界タイプ");
    this->face_boundary_button   .setText("登録済み境界条件リスト");
    this->edge_all_divide_label  .setText("分割数");
    this->edge_add_grading_button.setText("+");
    this->edge_remove_grading_button.setText("-");
    this->edge_all_divide_spin   .setMaximum(100000);
    this->edge_all_divide_spin   .setMinimum(1);

    //コンボボックス指定
    for(Boundary boundary: BoundaryDefinitionDialog::boundary_list){
        this->face_boundary_combo.addItem(boundary.name + " <" + Boundary::BoundaryTypeToString(boundary.type) + ">");
    }

    this->ui->Grid->addWidget(&this->name_label                 ,0,0);
    this->ui->Grid->addWidget(&this->name_edit                  ,0,1);
    this->ui->Grid->addWidget(&this->face_boundary_label        ,1,0);
    this->ui->Grid->addWidget(&this->face_boundary_combo        ,1,1);
    this->ui->Grid->addWidget(&this->face_boundary_button       ,2,1,1,2);
    this->ui->Grid->addWidget(&this->edge_all_divide_label      ,2,0);
    this->ui->Grid->addWidget(&this->edge_all_divide_spin       ,2,1);
    this->ui->Grid->addWidget(&this->edge_multi_grading_table   ,3,0,1,2);
    this->ui->Grid->addWidget(&this->edge_add_grading_button    ,4,0);
    this->ui->Grid->addWidget(&this->edge_remove_grading_button ,4,1);

    connect(&edge_add_grading_button   ,SIGNAL(pressed())               ,this,SLOT(AddMultiGradingTable()));
    connect(&edge_remove_grading_button,SIGNAL(pressed())               ,this,SLOT(RemoveMultiGradingTable()));
    connect(&name_edit              ,SIGNAL(textChanged(QString))    ,this,SLOT(LineEditChanged(QString)));
    connect(&face_boundary_combo    ,SIGNAL(currentIndexChanged(int)),this,SLOT(ComboChanged(int)));
}
PropertyDefinitionDialog::~PropertyDefinitionDialog()
{
    delete ui;
}

void PropertyDefinitionDialog::UpdateLayout(){
    //レイアウト解除
    disconnect(&name_edit          ,SIGNAL(textChanged(QString))    ,this,SLOT(LineEditChanged(QString)));
    disconnect(&face_boundary_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(ComboChanged(int)));
    this->name_label           .hide();
    this->name_edit            .hide();
    this->face_boundary_label  .hide();
    this->face_boundary_combo  .hide();
    this->face_boundary_button .hide();
    this->edge_all_divide_label.hide();
    this->edge_all_divide_spin .hide();
    this->edge_add_grading_button.hide();
    this->edge_remove_grading_button.hide();
    this->edge_multi_grading_table.hide();

    //表示タイプ選定
    QList<CObject*> selected = this->model->GetSelected();
    if     (selected.size() > 0 && std::all_of(selected.begin(),selected.end(),[](CObject* obj){return obj->is<CEdge>();}))this->ConstructEdge();
    else if(selected.size() > 0 && std::all_of(selected.begin(),selected.end(),[](CObject* obj){return obj->is<CFace>();}))this->ConstructFace();
    else{
        this->resize(100,20);
        this->constructed = CONSTRUCTED::EMPTY;
    }

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
        if(exist(selected,this->model->origin) ||
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
    this->repaint();
    connect(&name_edit          ,SIGNAL(textChanged(QString))    ,this,SLOT(LineEditChanged(QString)));
    //connect(&edge_divide_spin   ,SIGNAL(valueChanged(int))       ,this,SLOT(SpinChanged(int)));
    //connect(&edge_grading_spin  ,SIGNAL(valueChanged(double))    ,this,SLOT(DoubleSpinChanged(double)));
    connect(&face_boundary_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(ComboChanged(int)));

}

void PropertyDefinitionDialog::ShowBoundayDefinitionDialog(){
    BoundaryDefinitionDialog* diag = new BoundaryDefinitionDialog();
    diag->SetModel(model);
    diag->exec();
    disconnect(&face_boundary_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(ComboChanged(int)));

    //コンボ状態保存
    int save = this->face_boundary_combo.currentIndex();

    //コンボ更新
    this->face_boundary_combo.clear();
    for(Boundary boundary: BoundaryDefinitionDialog::boundary_list){
        this->face_boundary_combo.addItem(boundary.name + " <" + Boundary::BoundaryTypeToString(boundary.type) + ">");
    }
    //コンボ状態復元
    this->face_boundary_combo.setCurrentIndex(save);
    connect(&face_boundary_combo,SIGNAL(currentIndexChanged(int)),this,SLOT(ComboChanged(int)));
}

void PropertyDefinitionDialog::Accept(){
    //適用処理
    QList<CObject*> selected = this->model->GetSelected();

    //更新停止対象選定
    QList<CObject*> paused;
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
            if(this->edge_all_divide_spin .value() != 0)dynamic_cast<CEdge*>(obj)->setDivide (this->edge_all_divide_spin.value());
            //メッシュ寄せ係数
            CEdge::Grading g;
            double sum_dir =0,sum_cell =0;
            for(int i =0;i<this->edge_divide_spins.size();i++){
                sum_dir  += this->edge_dir_spins[i]   ->value();
                sum_cell += this->edge_divide_spins[i]->value();
            }
            for(int i =0;i<this->edge_divide_spins.size();i++){
                CEdge::Grading::GradingElement elm;
                elm.dir     = this->edge_dir_spins[i]    ->value() / sum_dir;
                elm.cell    = this->edge_divide_spins[i] ->value() / sum_cell;
                elm.grading = this->edge_grading_spins[i]->value();
                g.elements.push_back(elm);
            }
            dynamic_cast<CEdge*>(obj)->setGrading(g);
        }
    }

    //更新再開
    for(CObject* obj:paused) obj->ObserveRestart();

    emit RepaintRequest();
    this->ui->ApplyButton->setText("Apply");
}
void PropertyDefinitionDialog::AddMultiGradingTable(){
    //1行追加
    int rows = this->edge_multi_grading_table.rowCount();
    this->edge_multi_grading_table.setRowCount(rows+1);

    //新規スピンボックス設定
    this->edge_dir_spins    .push_back(new QDoubleSpinBox());
    this->edge_divide_spins .push_back(new QDoubleSpinBox());
    this->edge_grading_spins.push_back(new QDoubleSpinBox());

    //値設定
    this->SetupDoubleSpin(this->edge_dir_spins    .last(),
                          this->edge_divide_spins .last(),
                          this->edge_grading_spins.last());
    this->edge_dir_spins    .last()->setValue(0.0);
    this->edge_divide_spins .last()->setValue(0.0);
    this->edge_grading_spins.last()->setValue(1.0);

    //新規設定
    this->edge_multi_grading_table.setCellWidget(rows,0,this->edge_dir_spins.last());
    this->edge_multi_grading_table.setCellWidget(rows,1,this->edge_divide_spins.last());
    this->edge_multi_grading_table.setCellWidget(rows,2,this->edge_grading_spins.last());

    this->resize(this->width(),this->height() + this->edge_multi_grading_table.rowHeight(0));
}
void PropertyDefinitionDialog::RemoveMultiGradingTable(){
    QList<QTableWidgetSelectionRange> range = this->edge_multi_grading_table.selectedRanges();
    QVector<int> indexes;
    for(QTableWidgetSelectionRange r:range){
        for(int i = r.topRow();i <= r.bottomRow();i++){
            if(!exist(indexes,i) && i != 0){
                indexes.push_back(i);
            }
        }
    }
    //ソートする
    unique(indexes);
    std::sort(indexes.begin(),indexes.end(),std::greater<int>());
    //削除
    for(int i : indexes){
        this->edge_multi_grading_table.removeRow(i);
        this->edge_dir_spins.removeAt(i);
        this->edge_divide_spins.removeAt(i);
        this->edge_grading_spins.removeAt(i);
        this->resize(this->width(),this->height() - this->edge_multi_grading_table.rowHeight(0));
    }
}

void PropertyDefinitionDialog::Changed(){
    this->ui->ApplyButton->setText("*Apply");
}
void PropertyDefinitionDialog::LineEditChanged(QString){
    this->Changed();
}
void PropertyDefinitionDialog::SpinChanged(int){
    this->Changed();
}
void PropertyDefinitionDialog::DoubleSpinChanged(double){
    this->Changed();
}
void PropertyDefinitionDialog::ComboChanged(int){
    this->Changed();
}

