#include "MainWindow.h"
#include "ui_MainWindow.h"


void MainWindow::SetModel(CadModelCore* model){
    this->model = model;
    this->ui->SolidEdit ->SetModel(this->model);
    this->ui->ObjectTree->SetModel(this->model);
    this->move_diag->     SetModel(this->model);
    this->prop_diag->     SetModel(this->model);
    //モデルと結合
    connect(this->model,SIGNAL(UpdateSelected()),this,SLOT(RefreshUI()));
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->ObjectDock->resize(ui->ObjectDock->minimumSize());

    //ボタン関係
    connect(this ,SIGNAL(ToggleChanged(MAKE_OBJECT)),ui->SolidEdit ,SLOT(SetState(MAKE_OBJECT)));
    connect(ui->actionDelete     ,SIGNAL(triggered()) ,this,SLOT(Delete()));
    connect(ui->actionProperty   ,SIGNAL(triggered()) ,this,SLOT(ShowProperty()));
    connect(ui->actionMove       ,SIGNAL(triggered()) ,this,SLOT(ShowMoveTransform()));
    connect(ui->actionGridFilter ,SIGNAL(triggered()) ,this,SLOT(ShowGridFilter()));
    connect(ui->ToolBlocks       ,SIGNAL(triggered()) ,this,SLOT(MakeBlock()));
    connect(ui->ToolFace         ,SIGNAL(triggered()) ,this,SLOT(MakeFace()));
    connect(ui->actionSave       ,SIGNAL(triggered()) ,this,SLOT(Save()));
    connect(ui->actionOpen       ,SIGNAL(triggered()) ,this,SLOT(Load()));

    //リスト変更系
    connect(ui->RestraintList ,SIGNAL(itemSelectionChanged()) ,this ,SLOT(MakeRestraint()));

    //SolidEditFoamにイベントフィルター導入
    this->installEventFilter(ui->SolidEdit);
    connect(this->ui->SolidEdit,SIGNAL(MousePosChanged(Pos)),this,SLOT(RefreshStatusBar(Pos)));

    //移動ダイアログ関係
    move_diag = new MoveTransformDialog(this);
    this->installEventFilter(move_diag);
    connect(move_diag,SIGNAL(RepaintRequest()),this,SLOT(repaint()));
    connect(move_diag,SIGNAL(RepaintRequest()),ui->SolidEdit,SLOT(repaint()));

    //プロパティダイアログ関係
    prop_diag = new PropertyDefinitionDialog(this);
    connect(prop_diag,SIGNAL(RepaintRequest()),this,SLOT(repaint()));
    connect(prop_diag,SIGNAL(RepaintRequest()),ui->SolidEdit,SLOT(repaint()));

    //ドック関係
    connect(this->ui->actionShowObjectList,SIGNAL(triggered()),this,SLOT(ShowObjectList()));
    connect(this->ui->actionExport        ,SIGNAL(triggered()),this,SLOT(Export()));

    ConnectSignals();
    ui->ToolBlocks->setEnabled(false);
    CObject::drawing_scale = 1.0;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent  (QKeyEvent* event){
    this->ui->SolidEdit->keyPressEvent(event);

    //ESC押下時
    if(event->key() == Qt::Key_Escape)ClearButton();
}

void MainWindow::keyReleaseEvent(QKeyEvent* event){
    this->ui->SolidEdit->keyReleaseEvent(event);
}


void MainWindow::CtrlZ(){
}
void MainWindow::Delete(){
    for(CObject* obj : this->model->GetSelected()){
        this->model->Delete(obj);
    }

    this->ui->ObjectTree->UpdateObject();
    this->ui->SolidEdit->repaint();
    this->model->SelectedClear();
    repaint();
    RefreshUI();
}

void MainWindow::ConnectSignals(){
    connect(ui->ToolPoint ,SIGNAL(toggled(bool)),this,SLOT(ToggledPoint(bool)));
    connect(ui->ToolArc   ,SIGNAL(toggled(bool)),this,SLOT(ToggledArc(bool)));
    connect(ui->ToolLine  ,SIGNAL(toggled(bool)),this,SLOT(ToggledLine(bool)));
    //connect(ui->ToolRect  ,SIGNAL(toggled(bool)),this,SLOT(ToggledRect(bool)));
    connect(ui->ToolSpline,SIGNAL(toggled(bool)),this,SLOT(ToggledSpline(bool)));
}

void MainWindow::DisconnectSignals(){
    disconnect(ui->ToolPoint ,SIGNAL(toggled(bool)),this,SLOT(ToggledPoint(bool)));
    disconnect(ui->ToolArc   ,SIGNAL(toggled(bool)),this,SLOT(ToggledArc(bool)));
    disconnect(ui->ToolLine  ,SIGNAL(toggled(bool)),this,SLOT(ToggledLine(bool)));
    //disconnect(ui->ToolRect  ,SIGNAL(toggled(bool)),this,SLOT(ToggledRect(bool)));
    disconnect(ui->ToolSpline,SIGNAL(toggled(bool)),this,SLOT(ToggledSpline(bool)));
}

void MainWindow::ClearButton(){
    if(ui->ToolPoint ->isChecked())ui->ToolPoint ->setChecked(false);
    if(ui->ToolLine  ->isChecked())ui->ToolLine  ->setChecked(false);
    if(ui->ToolArc   ->isChecked())ui->ToolArc   ->setChecked(false);
    if(ui->ToolSpline->isChecked())ui->ToolSpline->setChecked(false);
}
void MainWindow::RefreshUI(){

    ui->RestraintList->clear();

    QVector<RestraintType> able = Restraint::Restraintable(this->model->GetSelected());
    for(RestraintType r:able){
        std::pair<std::string,std::string> p;
        if(r == MATCH     )p = std::make_pair("一致"   ,":/Restraint/MatchRestraint.png");
        if(r == EQUAL     )p = std::make_pair("等値"   ,":/Restraint/EqualRestraint.png");
        if(r == CONCURRENT)p = std::make_pair("並行"   ,":/Restraint/ConcurrentRestraint.png");
        if(r == VERTICAL  )p = std::make_pair("垂直"   ,":/Restraint/VerticalRestraint.png");
        if(r == HORIZONTAL)p = std::make_pair("水平"   ,":/Restraint/HorizontalRestraint.png");
        if(r == TANGENT   )p = std::make_pair("正接"   ,":/Restraint/TangentRestraint.png");
        if(r == LOCK      )p = std::make_pair("固定"   ,":/Restraint/LockRestraint.png");
        if(r == UNLOCK    )p = std::make_pair("固定解除",":/Restraint/UnlockRestraint.png");
        if(r == MARGE     )p = std::make_pair("マージ"  ,":/Restraint/Marge.png");
        ui->RestraintList->addItem(new QListWidgetItem(p.first.c_str()));
        ui->RestraintList->item(ui->RestraintList->count()-1)->setIcon(QIcon(p.second.c_str()));
    }

    //ブロック生成可否判定
    ui->ToolBlocks->setEnabled(CBlock::Creatable(this->model->GetSelected()));
    //ブロック生成可否判定
    ui->ToolFace->setEnabled(CFace::Creatable(this->model->GetSelected()));
    //スマート寸法は1つから
    ui->ToolDimension->setEnabled(this->model->GetSelected().size() >= 1);
    //リスト要素数で出力ボタンの無効化を決定
    //ui->ExportButton->setEnabled(this->ui->BlockList->count() > 0);

    this->repaint();
}

#define ToggledToolDefinition(TYPE)             \
void MainWindow::Toggled##TYPE (bool checked){  \
    if(MAKE_OBJECT::TYPE != MAKE_OBJECT::Edit)DisconnectSignals();        \
    if(checked){                                \
        ClearButton();                          \
        emit ToggleChanged(MAKE_OBJECT::TYPE);  \
        ui->Tool##TYPE->setChecked(true);       \
    }else{                                      \
        emit ToggleChanged(MAKE_OBJECT::Edit);  \
    }                                           \
    ConnectSignals();                           \
}                                               \

ToggledToolDefinition(Point)
ToggledToolDefinition(Line)
ToggledToolDefinition(Arc)
//ToggledToolDefinition(Rect)
ToggledToolDefinition(Spline)

void MainWindow::ToggleConflict(bool conflict){
    if(conflict)this->ui->actionCheckConflict->setIcon(QIcon(":/Others/Conflict.png"));
    else        this->ui->actionCheckConflict->setIcon(QIcon(":/Others/NotConflict.png"));
}
void MainWindow::ShowProperty(){
    prop_diag->UpdateLayout();
    prop_diag->show();
}

void MainWindow::ShowMoveTransform(){
    move_diag->show();
}
void MainWindow::ShowGridFilter(){
    static GridFilterDialog* diag = new GridFilterDialog(this);
    //connect(diag,SIGNAL(ChangeGri dStatus(double,double)),ui->CadEdit,SLOT(SetGridFilterStatus(double,double)));
    diag->setWindowTitle("GridFilter");
    diag->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    diag->show();
}

void MainWindow::MakeRestraint(){
    //qDebug() << text;
    //RestraintType type = Paradox;
    /*
    if(ui->RestraintList->currentItem()->text() == "一致")type = MATCH;
    if(ui->RestraintList->currentItem()->text() == "並行")type = CONCURRENT;
    if(ui->RestraintList->currentItem()->text() == "垂直")type = VERTICAL;
    if(ui->RestraintList->currentItem()->text() == "水平")type = HORIZONTAL;
    if(ui->RestraintList->currentItem()->text() == "正接")type = TANGENT;
    if(ui->RestraintList->currentItem()->text() == "固定")type = LOCK;
    if(ui->RestraintList->currentItem()->text() == "固定解除")type = UNLOCK;
    ui->RestraintList->clear();
    this->RefreshUI();
    */
}

void MainWindow::MakeBlock(){
    CBoxDefineDialog* diag = new CBoxDefineDialog();

    diag->SetModel(this->model);

    //すでに作成済みのブロックであれば
    if(this->model->GetSelected().size() == 1 && this->model->GetSelected()[0]->is<CBlock>()){
        //定義編集
        diag->block = dynamic_cast<CBlock*>(this->model->GetSelected()[0]);
        diag->ImportCBlock();
    }else{
        //新規定義
        CBlock* block = new CBlock(this);
        for(QObject* obj: this->model->GetSelected()){
            block->faces.push_back(dynamic_cast<CFace*>(obj));
        }
        //コネクト
        for(int i=0;i<12;i++){
            connect(block->GetPointSequence(i),SIGNAL(Moved()),block,SLOT(RefreshDividePoint()));
        }
        diag->block = block;
        diag->ImportCBlock();
        block->RefreshDividePoint();
    }

    //ダイアログ起動
    if(diag->exec()){
        diag->ExportCBlock();
        if(!exist(this->model->GetBlocks(),diag->block)){
            this->model->AddBlocks(diag->block);
        }
        this->model->SelectedClear();//選択解除
    }
    RefreshUI();
}
void MainWindow::MakeFace(){
    CFace* face = new CFace(this);
    for(QObject* obj: this->model->GetSelected()){
        face->edges.push_back(dynamic_cast<CEdge*>(obj));
    }
    this->model->AddFaces(face);
    this->model->GetSelected().clear();//選択解除
}

void MainWindow::RefreshStatusBar(Pos pos){
    this->ui->statusBar->showMessage(QString("( %1,%2,%3)").arg(QString::number(pos.x() ,'f',3))
                                                           .arg(QString::number(pos.y() ,'f',3))
                                                           .arg(QString::number(pos.z() ,'f',3)));
}

void MainWindow::ShowObjectList(){
    this->ui->ObjectDock->show();
}

void MainWindow::Save(){
    QString filepath = QFileDialog::getSaveFileName(this,
                                                    "Save file",
                                                    "",
                                                    "FoamCAD File(*.foamcad);;All Files (*)");
    this->model->ExportFoamFile(filepath);
}

void MainWindow::Load(){
    QString filepath = QFileDialog::getOpenFileName(this,
                                                    "Load file",
                                                    "",
                                                    "FoamCAD File(*.foamcad);;All Files (*)");
    this->model->ImportFoamFile(filepath);

}

void MainWindow::Export(){
    ExportDialog* diag = new ExportDialog();
    diag->SetModel(this->model);
    diag->exec();

}


