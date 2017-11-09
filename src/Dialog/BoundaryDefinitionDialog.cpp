#include "BoundaryDefinitionDialog.h"

//初期値設定
QVector<Boundary> BoundaryDefinitionDialog::boundary_list =
    {{CFace().getBoundary().name,CFace().getBoundary().type}};

void BoundaryDefinitionDialog::SetModel(CadModelCore *m){
    this->model = m;
    LoadTable();
    connect(this->model,SIGNAL(UpdateAnyObject()),this,SLOT(LoadTable()));
    connect(this->table,SIGNAL(cellChanged(int,int)),this,SLOT(CellChanged(int,int)));
}
void BoundaryDefinitionDialog::SaveTable(){
    //状態を保存
    this->boundary_list.clear();
    for(int i=0;i<this->table->rowCount();i++){
        Boundary bound;
        bound.name = this->table->item(i,0)->text();
        bound.type = Boundary::StringToBoundaryType(dynamic_cast<QComboBox*>(this->table->cellWidget(i,1))->currentText().split(" ")[0]);
        if(bound.name == "")bound.name = "Noname";
        this->boundary_list.push_back(bound);
    }
}
void BoundaryDefinitionDialog::LoadTable(){
    disconnect(this->table,SIGNAL(cellChanged(int,int)),this,SLOT(CellChanged(int,int)));

    //表示更新
    this->table->clear();
    //テーブル構築
    this->table->setColumnCount(3);
    this->table->setRowCount(this->boundary_list.size());
    this->table->setHorizontalHeaderItem(0,new QTableWidgetItem("境界名"));
    this->table->setHorizontalHeaderItem(1,new QTableWidgetItem("境界タイプ"));
    this->table->setHorizontalHeaderItem(2,new QTableWidgetItem("参照数"));
    this->table->setColumnWidth(0,120);
    this->table->setColumnWidth(1,200);
    this->table->setColumnWidth(2,50);
    this->resize(450,400);
    for(int i=0;i<this->boundary_list.size();i++ ){
        QComboBox* combo = new QComboBox(this);
        //コンボ設定
        for(QString str :this->boundary_combo_text){
            combo->addItem(str);
            if(str == this->boundary_combo_text[static_cast<int>(this->boundary_list[i].type)]){
                combo->setCurrentText(str);
            }
        }

        //参照数カウント
        int ref_count = 0;
        if(this->model != nullptr){
            for(CFace* face:this->model->GetFaces()){
                if(!exist(CFace::base,face) &&
                   face->getBoundary().name == this->boundary_list[i].name &&
                   face->getBoundary().type == this->boundary_list[i].type ){
                    ref_count++;
                }
            }
        }
        this->table->setItem      (i,0,new QTableWidgetItem(this->boundary_list[i].name));
        this->table->setCellWidget(i,1,combo);
        this->table->setItem      (i,2,new QTableWidgetItem(QString::number(ref_count)));
        this->table->item(i,2)->setFlags(this->table->item(i,2)->flags() & Qt::ItemIsEditable);


        //境界条件未定義<undefined>は変更不可
        if(this->boundary_list[i].name == CFace().getBoundary().name &&
           this->boundary_list[i].type == CFace().getBoundary().type ){
            this->table->item(i,0)->setFlags(this->table->item(i,0)->flags() & Qt::ItemIsEditable);
            combo->setDisabled(true);
        }

        connect(combo,SIGNAL(currentIndexChanged(int)),this,SLOT(ComboChanged(int)));
    }
    connect(this->table,SIGNAL(cellChanged(int,int)),this,SLOT(CellChanged(int,int)));
}

BoundaryDefinitionDialog::BoundaryDefinitionDialog()
{
    //レイアウト設定
    this->setWindowTitle("Boundary List");
    this->plus  = new QPushButton("+");
    this->minus = new QPushButton("-");
    this->table = new QTableWidget();
    QVBoxLayout* v_l = new QVBoxLayout();
    QHBoxLayout* h_l = new QHBoxLayout();
    h_l->addWidget(plus);
    h_l->addWidget(minus);
    v_l->addWidget(table);
    v_l->addLayout(h_l);
    this->setLayout(v_l);
    connect(this->plus ,SIGNAL(pressed()),this,SLOT(PlusButtonPushed ()));
    connect(this->minus,SIGNAL(pressed()),this,SLOT(MinusButtonPushed()));
}
void BoundaryDefinitionDialog::PlusButtonPushed(){
    QDialog* diag = new QDialog(this);
    //ダイアログ構築
    QGridLayout* grid = new QGridLayout();
    QLineEdit*   edit = new QLineEdit();
    QComboBox*  combo = new QComboBox();
    QPushButton* push = new QPushButton("追加");

    //コンボ設定
    for(QString str :this->boundary_combo_text){
        combo->addItem(str);
    }
    //コネクト
    connect(push,SIGNAL(pressed()),diag,SLOT(accept()));

    grid->addWidget(new QLabel("境界名　　："),0,0);
    grid->addWidget(edit                    ,0,1);
    grid->addWidget(new QLabel("境界タイプ："),1,0);
    grid->addWidget(combo                   ,1,1);
    grid->addWidget(push                    ,2,1);

    diag->setWindowTitle("New Boundary");
    diag->setLayout(grid);
    if(diag->exec()){
        //新要素追加
        Boundary bound;
        bound.type = static_cast<Boundary::Type>(combo->currentIndex());
        bound.name = edit->text();
        this->boundary_list.push_back(bound);
        //再読み込み
        LoadTable();
    }
}
void BoundaryDefinitionDialog::MinusButtonPushed(){
    QList<QTableWidgetSelectionRange> range = this->table->selectedRanges();
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
        if(this->table->item(i,2)->text().toInt() == 0){
            this->table->removeRow(i);
        }
    }
    //保存
    SaveTable();
}
void BoundaryDefinitionDialog::ComboChanged(int){
    SaveTable();
}
void BoundaryDefinitionDialog::CellChanged(int,int){
    SaveTable();
}

