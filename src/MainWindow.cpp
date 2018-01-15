#include "MainWindow.h"
#include "ui_MainWindow.h"


void MainWindow::SetModel(CadModelCore* model){
    //モデルと結合
    this->model = model;

    //各UIに伝達
    this->ui->ObjectTree->SetModel(this->model);
    this->ui->SolidEdit ->SetModel(this->model);
    //ダイアログに伝達
    this->move_diag     ->SetModel(this->model);
    this->prop_diag     ->SetModel(this->model);

    //検索モデルに伝達
    this->search.SetModel(model);

    //競合リストに伝達
    this->conf_list->SetModel(model);

    //選択オブジェクトが変更時にUIを更新する。
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
    connect(ui->actionDelete     ,SIGNAL(triggered()) ,this,SLOT(Delete()));           //オブジェクト削除
    connect(ui->actionProperty   ,SIGNAL(triggered()) ,this,SLOT(ShowProperty()));     //プロパティダイアログ表示
    connect(ui->actionMove       ,SIGNAL(triggered()) ,this,SLOT(ShowMoveTransform()));//平行移動ダイアログ表示
    connect(ui->ToolFace         ,SIGNAL(triggered()) ,this,SLOT(MakeFace()));         //面作成ボタン
    connect(ui->ToolBlock        ,SIGNAL(triggered()) ,this,SLOT(MakeBlock()));        //立体作成ボタン
    connect(ui->ToolProjection   ,SIGNAL(triggered()) ,this,SLOT(StartProjection()));  //投影ボタン
    connect(ui->actionSave       ,SIGNAL(triggered()) ,this,SLOT(Save()));             //保存ボタン
    connect(ui->actionOpen       ,SIGNAL(triggered()) ,this,SLOT(Load()));             //開くボタン

    //ダイアログ生成
    this->move_diag = new MoveTransformDialog(this);
    this->prop_diag = new PropertyDefinitionDialog(this);
    this->ui->SolidEdit->move_diag = this->move_diag; //SolidEditFoamと移動ダイアログを共有

    //イベントフィルター
    this->installEventFilter(this->ui->SolidEdit);
    this->installEventFilter(this->move_diag);

    //リスト変更系
    connect(this->ui->RestraintList ,SIGNAL(itemClicked(QListWidgetItem*)) ,this ,SLOT(MakeRestraint(QListWidgetItem*)));

    //SolidEditFoam関係
    connect(this->ui->SolidEdit,SIGNAL(MousePosChanged(Pos)),this,SLOT(RefreshStatusBar(Pos))); // 座標監視

    //移動ダイアログ関係
    connect(this->move_diag,SIGNAL(RepaintRequest()),this,SLOT(repaint()));
    connect(this->move_diag,SIGNAL(RepaintRequest()),ui->SolidEdit,SLOT(repaint()));

    //プロパティダイアログ関係
    connect(prop_diag,SIGNAL(RepaintRequest()),this,SLOT(repaint()));
    connect(prop_diag,SIGNAL(RepaintRequest()),ui->SolidEdit,SLOT(repaint()));

    //ドック関係
    connect(this->ui->actionShowObjectList   ,SIGNAL(triggered()),this,SLOT(ShowObjectList()));    //オブジェクトリスト表示
    connect(this->ui->actionShowRestraintList,SIGNAL(triggered()),this,SLOT(ShowRestraintList())); //幾何拘束リスト表示
    connect(this->ui->actionShowConflictList ,SIGNAL(triggered()),this,SLOT(ShowConflictList()));  //競合リスト表示
    connect(this->ui->actionExport           ,SIGNAL(triggered()),this,SLOT(Export())); //出力ダイアログ
    this->ui->ConflictDeck->hide();//競合リストを標準で不可視

    //ツールをコネクト
    ConnectSignals();
    //競合リストを作成
    this->conf_list = new ConflictList();
    this->conf_list->SetWidget(this->ui->ConflictList);
    connect(this->conf_list,SIGNAL(Changed()),ui->SolidEdit,SLOT(repaint()));

    ui->ToolBlock->setEnabled(false);
    ui->ToolFace->setEnabled(false);
    CObject::drawing_scale = 1.0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent  (QKeyEvent* event){
    this->ui->SolidEdit->keyPressEvent(event);
    if(event->key() == Qt::Key_Shift)this->shift_press = true;

    //ESC押下時
    if(event->key() == Qt::Key_Escape)ClearButton();
}

void MainWindow::keyReleaseEvent(QKeyEvent* event){
    if(event->key() == Qt::Key_Shift)this->shift_press = false;
    this->ui->SolidEdit->keyReleaseEvent(event);
}


void MainWindow::CtrlZ(){
}
void MainWindow::Delete(){
    if(this->shift_press){
        QList<CPoint*> pp;
        for(CObject* obj : this->model->GetSelected()){
            for(CPoint* p:obj->GetAllChildren()){
                pp.push_back(p);
            }
        }
        for(CPoint* obj : pp){
            this->model->Delete(obj);
        }
    }else{
        for(CObject* obj : this->model->GetSelected()){
            this->model->Delete(obj);
        }
    }

    this->ui->ObjectTree->UpdateObject();
    this->ui->SolidEdit->repaint();
    this->model->SelectedClear();
    repaint();
    RefreshUI();
}

void MainWindow::ConnectSignals(){
    connect(ui->ToolPoint     ,SIGNAL(toggled(bool))  ,this,SLOT(ToggledPoint(bool)));
    connect(ui->ToolArc       ,SIGNAL(toggled(bool))  ,this,SLOT(ToggledArc(bool)));
    connect(ui->ToolLine      ,SIGNAL(toggled(bool))  ,this,SLOT(ToggledLine(bool)));
    connect(ui->ToolSpline    ,SIGNAL(toggled(bool))  ,this,SLOT(ToggledSpline(bool)));
    connect(ui->ToolFileEdge  ,SIGNAL(triggered(bool)),this,SLOT(ToggledFileEdge(bool)));
    connect(ui->ToolSTL       ,SIGNAL(triggered(bool)),this,SLOT(ToggledSTL(bool)));
}

void MainWindow::DisconnectSignals(){
    disconnect(ui->ToolPoint   ,SIGNAL(toggled(bool)),this,SLOT(ToggledPoint(bool)));
    disconnect(ui->ToolArc     ,SIGNAL(toggled(bool)),this,SLOT(ToggledArc(bool)));
    disconnect(ui->ToolLine    ,SIGNAL(toggled(bool)),this,SLOT(ToggledLine(bool)));
    disconnect(ui->ToolSpline  ,SIGNAL(toggled(bool)),this,SLOT(ToggledSpline(bool)));
    disconnect(ui->ToolFileEdge,SIGNAL(triggered(bool)),this,SLOT(ToggledFileEdge(bool)));
    disconnect(ui->ToolSTL     ,SIGNAL(triggered(bool)),this,SLOT(ToggledSTL(bool)));
}

void MainWindow::ClearButton(){
    if(ui->ToolPoint ->isChecked())ui->ToolPoint ->setChecked(false);
    if(ui->ToolLine  ->isChecked())ui->ToolLine  ->setChecked(false);
    if(ui->ToolArc   ->isChecked())ui->ToolArc   ->setChecked(false);
    if(ui->ToolSpline->isChecked())ui->ToolSpline->setChecked(false);
}
void MainWindow::RefreshUI(){

    ui->RestraintList->clear();

    this->creatable = Restraint::Restraintable(this->model->GetSelected());
    for(Restraint* r:this->creatable){
        ui->RestraintList->addItem(new QListWidgetItem(r->GetRestraintName()));
        ui->RestraintList->item(ui->RestraintList->count()-1)->setIcon(QIcon(r->GetIconPath()));
    }

    //ボタン有効化/無効化
    QList<CEdge*> edges;
    QList<CFace*> faces;
    for(CObject* obj:this->model->GetSelected())if(obj->is<CEdge>())edges.push_back(dynamic_cast<CEdge*>(obj));
    for(CObject* obj:this->model->GetSelected())if(obj->is<CFace>())faces.push_back(dynamic_cast<CFace*>(obj));
    ui->ToolFace      ->setEnabled(search.SearchEdgeMakeFace(edges) .size() > 0);   //面作成ボタン
    ui->ToolBlock     ->setEnabled(search.SearchFaceMakeBlock(faces).size() > 0);   //立体作成ボタン
    ui->ToolProjection->setEnabled(search.Projectable(this->model->GetSelected())); //投影ボタン

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
ToggledToolDefinition(Spline)

void MainWindow::ToggledFileEdge(bool){
    //ファイルパス変更ダイアログ
    QString filepath = QFileDialog::getOpenFileName(this,"外部ファイルを選択","","CSV File(*.csv);;All Files (*)");
    if(filepath != ""){
        CFileEdge* edge = CFileEdge::CreateFromFile(filepath);
        if(edge != nullptr){
            this->model->AddEdges(edge);
            for(CPoint* p:edge->GetAllChildren()){
                this->model->AddObject(p);
            }
        }
    }
}
void MainWindow::ToggledSTL(bool){
    //ファイルパス変更ダイアログ
    QString filepath = QFileDialog::getOpenFileName(this,
                                                    "STLファイルを選択",
                                                    "",
                                                    "STL File(*.stl);;All Files (*)");
    if(filepath != ""){
        CStl* stl = CStl::CreateFromFile(filepath);
        if(stl != nullptr){
            this->model->AddStls(stl);
            this->model->ObservePause();
            for(int i =0;i<stl->points.size();i++){
                this->model->AddPoints(stl->points[i]);
            }/*
            for(int i =0;i<stl->edges.size();i++){
                this->model->AddEdges (stl->edges[i]);
            }*/
            this->model->ObserveRestart();
        }
    }
    this->ui->SolidEdit->repaint();
}

void MainWindow::ShowProperty(){
    prop_diag->UpdateLayout();
    prop_diag->show();
}

void MainWindow::ShowMoveTransform(){
    this->move_diag->show();
}

void MainWindow::MakeRestraint(QListWidgetItem*){
    //qDebug() << text;
    Restraint* rest = nullptr;
    int index = ui->RestraintList->currentRow();
    rest = this->creatable[index];
    rest->Create(this->model->GetSelected());

    /*
    if(ui->RestraintList->currentItem()->text() == "等値"){
        rest = new EqualLengthRestraint();
        CEdge* ee = dynamic_cast<CEdge*>(this->model->GetSelected().first());
        rest->Create(this->model->GetSelected());
    }*/

    if(rest != nullptr){
        this->model->AddRestraints(rest);
        rest->Calc();
        this->model->SelectedClear();
    }
    ui->RestraintList->clear();
    this->RefreshUI();

}

void MainWindow::MakeBlock(){
    CBlock* block = new CBlock(this);
    QList<CFace*> faces;
    for(CObject* obj:this->model->GetSelected()){
        faces.push_back(dynamic_cast<CFace*>(obj));
    }
    faces = this->search.SearchFaceMakeBlock(faces);
    //faceがモデルになければ追加しておく
    for(CFace* face:faces){
        if(!exist(this->model->GetFaces(),face)){
            this->model->AddFaces(face);
        }
    }
    block->Create(faces);
    this->model->AddBlocks(block);
    this->model->SelectedClear();//選択解除
}
void MainWindow::MakeFace(){
    CFace* face = new CFace(this);
    QList<CEdge*> edges;
    for(CObject* obj:this->model->GetSelected())if(obj->is<CEdge>())edges.push_back(dynamic_cast<CEdge*>(obj));
    face->Create(this->search.SearchEdgeMakeFace(edges));
    this->model->AddFaces(face);
    this->model->SelectedClear();//選択解除
}
void MainWindow::StartProjection(){
    CFace* face = this->search.CreateProjectionFace(this->model->GetSelected());
    face->SetContours(true);
    this->ui->SolidEdit->StartProjection(face);
    this->model->SelectedClear();//選択解除
    this->repaint();
}

void MainWindow::RefreshStatusBar(Pos pos){
    this->ui->statusBar->showMessage(QString("( %1,%2,%3)").arg(QString::number(pos.x() ,'f',3))
                                                           .arg(QString::number(pos.y() ,'f',3))
                                                           .arg(QString::number(pos.z() ,'f',3)));
}

void MainWindow::ShowObjectList(){
    this->ui->ObjectDock->show();
}
void MainWindow::ShowRestraintList(){
    this->ui->RestraintDeck->show();
}
void MainWindow::ShowConflictList(){
    this->ui->ConflictDeck->show();

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


