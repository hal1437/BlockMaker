#include "MainWindow.h"
#include "ui_MainWindow.h"


void MainWindow::SetModel(CadModelCore* model){
    this->model = model;
    this->ui->CadEdit  ->SetModel(model);
    this->ui->SolidEdit->SetModel(model);
    //モデルと結合
    connect(this->model,SIGNAL(UpdateEdges   (QVector<CEdge*>  )),this,SLOT(UpdateObjectTree        (QVector<CEdge*>)));
    connect(this->model,SIGNAL(UpdateSelected(QVector<CObject*>)),this,SLOT(UpdateObjectTreeSelected(QVector<CObject*>)));
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //connect(ui->actionCtrlZ          ,SIGNAL(triggered())                        ,this       ,SLOT(CtrlZ()));
    connect(ui->actionDelete         ,SIGNAL(triggered())                        ,this       ,SLOT(Delete()));
    connect(ui->actionMove           ,SIGNAL(triggered())                        ,this       ,SLOT(ShowMoveTransform()));
    connect(ui->actionSolidView      ,SIGNAL(triggered())                        ,this       ,SLOT(ShowSolidView()));
    connect(ui->actionGridFilter     ,SIGNAL(triggered())                        ,this       ,SLOT(ShowGridFilter()));
    connect(ui->actionResetExpantion ,SIGNAL(triggered())                        ,this       ,SLOT(ResetAllExpantion()));
    connect(ui->ToolDimension        ,SIGNAL(triggered())                        ,ui->CadEdit,SLOT(MakeSmartDimension()));
    connect(ui->ToolBlocks           ,SIGNAL(triggered())                        ,this       ,SLOT(MakeBlock()));
    connect(ui->BlockList            ,SIGNAL(itemDoubleClicked(QListWidgetItem*)),ui->CadEdit,SLOT(ConfigureBlock(QListWidgetItem*)));
    connect(ui->CadEdit              ,SIGNAL(ToggleConflict(bool))               ,this       ,SLOT(ToggleConflict(bool)));
    connect(ui->ExportButton         ,SIGNAL(pressed())                          ,ui->CadEdit,SLOT(Export()));
    connect(ui->DepthSpin            ,SIGNAL(valueChanged(double))               ,ui->CadEdit,SLOT(SetDepth(double)));

    //CadEditFoam関連
    connect(this          ,SIGNAL(ToggleChanged(MAKE_OBJECT)),ui->CadEdit   ,SLOT(SetState(MAKE_OBJECT)));
    connect(this          ,SIGNAL(ToggleChanged(MAKE_OBJECT)),ui->SolidEdit ,SLOT(SetState(MAKE_OBJECT)));
    connect(ui->CadEdit   ,SIGNAL(ScaleChanged(double)),ui->ScaleSpin ,SLOT(setValue(double)));
    connect(ui->CadEdit   ,SIGNAL(DepthChanged(double)),ui->DepthSpin ,SLOT(setValue(double)));
    connect(ui->CadEdit   ,SIGNAL(RequireRefreshUI())  ,this          ,SLOT(RefreshUI()));
    connect(ui->CadEdit   ,SIGNAL(MouseMoved(Pos))     ,this          ,SLOT(RefreshStatusBar(Pos)));
    connect(ui->actionSave,SIGNAL(triggered())         ,ui->CadEdit   ,SLOT(Save()));
    connect(ui->actionLoad,SIGNAL(triggered())         ,ui->CadEdit   ,SLOT(Load()));

    //リスト変更系
    connect(ui->RestraintList ,SIGNAL(itemSelectionChanged()) ,this ,SLOT(MakeRestraint()));
    connect(ui->ObjectList    ,SIGNAL(itemSelectionChanged()) ,this ,SLOT(ReciveObjectListChanged()));
    connect(ui->BlockList     ,SIGNAL(itemSelectionChanged()) ,this ,SLOT(ReciveBlockListChanged ()));

    //CadEditFoamにイベントフィルター導入
    this->installEventFilter(ui->CadEdit);
    this->installEventFilter(ui->SolidEdit);

    //RefreshUI();
    //ReciveObjectListChanged();
    //ReciveBlockListChanged();
    ConnectSignals();
    ui->ToolBlocks->setEnabled(false);
    ui->ObjectList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    CObject::drawing_scale = 1.0;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent  (QKeyEvent* event){
    this->ui->CadEdit->keyPressEvent(event);
    this->ui->SolidEdit->keyPressEvent(event);

    //ESC押下時
    if(event->key() == Qt::Key_Escape){
        this->ui->CadEdit->Escape();
        ClearButton();
        RefreshUI();
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent* event){
    this->ui->CadEdit->keyReleaseEvent(event);
}


void MainWindow::CtrlZ(){
    /*
    if(!log.empty()){
        ui->CadEdit->RemoveObject(log.back());
        log.erase(log.end()-1);
        creating_count=0;
    }*/
}
void MainWindow::Delete(){

    //ランダム回転関数に置き換え中
    static int c = 0;
    std::random_device rd;
    this->ui->SolidEdit->setCameraRotate(Mod((double)rd(),M_PI)-M_PI/2,Mod((double)rd(),2*M_PI));
    c++;

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
    //if(make_obj != nullptr && make_obj->isCreating())make_obj->Create(nullptr,-1);
    if(ui->ToolPoint ->isChecked())ui->ToolPoint ->setChecked(false);
    if(ui->ToolLine  ->isChecked())ui->ToolLine  ->setChecked(false);
    if(ui->ToolArc   ->isChecked())ui->ToolArc   ->setChecked(false);
    if(ui->ToolRect  ->isChecked())ui->ToolRect  ->setChecked(false);
    if(ui->ToolSpline->isChecked())ui->ToolSpline->setChecked(false);
}
void MainWindow::RefreshUI(){

    ui->RestraintList->clear();
    //ui->BlockList->clear();

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

    disconnect(ui->RestraintList ,SIGNAL(itemSelectionChanged()) ,this ,SLOT(MakeRestraint()));
    disconnect(ui->ObjectList    ,SIGNAL(itemSelectionChanged()) ,this ,SLOT(ReciveObjectListChanged()));
    disconnect(ui->BlockList     ,SIGNAL(itemSelectionChanged()) ,this ,SLOT(ReciveBlockListChanged ()));
    this->ui->CadEdit->ExportObjectList(this->ui->ObjectList);
    this->ui->CadEdit->ExportCBoxList  (this->ui->BlockList);
    connect(ui->RestraintList ,SIGNAL(itemSelectionChanged()) ,this ,SLOT(MakeRestraint()));
    connect(ui->ObjectList    ,SIGNAL(itemSelectionChanged()) ,this ,SLOT(ReciveObjectListChanged()));
    connect(ui->BlockList     ,SIGNAL(itemSelectionChanged()) ,this ,SLOT(ReciveBlockListChanged ()));

    //拘束更新
    ui->CadEdit->RefreshRestraints();
    //ブロック生成可否判定
    ui->ToolBlocks->setEnabled(CBlock::Creatable(this->model->GetSelected()));
    //スマート寸法は1つから
    ui->ToolDimension->setEnabled(this->model->GetSelected().size() >= 1);
    //リスト要素数で出力ボタンの無効化を決定
    ui->ExportButton->setEnabled(this->ui->BlockList->count() > 0);

    this->repaint();
}

#define ToggledToolDefinition(TYPE)             \
void MainWindow::Toggled##TYPE (bool checked){  \
    if(MAKE_OBJECT::TYPE != MAKE_OBJECT::Edit)DisconnectSignals();        \
    if(checked){                                \
        ClearButton();                          \
        emit ToggleChanged(MAKE_OBJECT::TYPE);               \
        ui->Tool##TYPE->setChecked(true);       \
    }else{                                      \
        emit ToggleChanged(MAKE_OBJECT::Edit);               \
    }                                           \
    ConnectSignals();                           \
}                                               \

ToggledToolDefinition(Point)
ToggledToolDefinition(Line)
ToggledToolDefinition(Arc)
//ToggledToolDefinition(Rect)
ToggledToolDefinition(Spline)

void MainWindow::ToggleConflict(bool conflict){
    if(conflict){
        this->ui->actionCheckConflict->setIcon(QIcon(":/Others/Conflict.png"));
    }else{
        this->ui->actionCheckConflict->setIcon(QIcon(":/Others/NotConflict.png"));
    }
}
void MainWindow::ResetAllExpantion(){
    CObject::drawing_scale = 1.0;
    this->ui->ScaleSpin->setValue(1.0);
    this->ui->CadEdit->ResetAllExpantion();
}

void MainWindow::ShowMoveTransform(){
    static MoveTransformDialog* diag = new MoveTransformDialog(this);
    connect(diag,SIGNAL(RepaintRequest()),this,SLOT(repaint()));
    connect(diag,SIGNAL(RepaintRequest()),ui->CadEdit  ,SLOT(repaint()));
    connect(diag,SIGNAL(RepaintRequest()),ui->SolidEdit,SLOT(repaint()));
    diag->SetModel(this->model);
    diag->setWindowTitle("MoveTransform");
    diag->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    diag->show();
}
void MainWindow::ShowSolidView(){
    static SolidView* s = nullptr;
    if(s == nullptr){
        s = new SolidView(this);
        connect(this->ui->CadEdit,SIGNAL(RequireRefreshSolidUI(QVector<CEdge*>,QVector<CBlock>)),
                s                ,SLOT  (RefreshUI            (QVector<CEdge*>,QVector<CBlock>)));
    }
    s->setWindowTitle("SolidView");
    s->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    s->setMinimumSize(300,300);
    s->show();
}
void MainWindow::ShowGridFilter(){
    static GridFilterDialog* diag = new GridFilterDialog(this);
    connect(diag,SIGNAL(ChangeGridStatus(double,double)),ui->CadEdit,SLOT(SetGridFilterStatus(double,double)));
    diag->setWindowTitle("GridFilter");
    diag->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
    diag->show();
}

void MainWindow::MakeRestraint(){
    //qDebug() << text;
    RestraintType type = Paradox;
    if(ui->RestraintList->currentItem()->text() == "一致")type = MATCH;
    if(ui->RestraintList->currentItem()->text() == "並行")type = CONCURRENT;
    if(ui->RestraintList->currentItem()->text() == "垂直")type = VERTICAL;
    if(ui->RestraintList->currentItem()->text() == "水平")type = HORIZONTAL;
    if(ui->RestraintList->currentItem()->text() == "正接")type = TANGENT;
    if(ui->RestraintList->currentItem()->text() == "固定")type = LOCK;
    if(ui->RestraintList->currentItem()->text() == "固定解除")type = UNLOCK;
    if(type != Paradox){
        ui->CadEdit->MakeRestraint(type);
    }
    if(ui->RestraintList->currentItem()->text() == "マージ"){
        ui->CadEdit->MergePoints();
    }
    ui->RestraintList->clear();
    this->RefreshUI();
}

void MainWindow::MakeBlock(){
    this->ui->CadEdit->MakeBlock();
    RefreshUI();
}

void MainWindow::ReciveObjectListChanged(){
    this->ui->CadEdit->ImportObjectList(this->ui->ObjectList);
    this->ui->CadEdit->repaint();
    RefreshUI();
}
void MainWindow::ReciveBlockListChanged(){
    this->ui->CadEdit->ImportCBoxList  (this->ui->BlockList);
    RefreshUI();
}
void MainWindow::RefreshStatusBar(Pos){
    Pos out;
    CObject* hanged = this->ui->CadEdit->getHanged();
    if(hanged == nullptr){
        //マウス位置のローカル座標
        out = this->ui->CadEdit->getMousePos();
    }else{
        //選択オブジェクトの最近点
        out = hanged->GetNearPos(this->ui->CadEdit->getMousePos());
    }
    this->ui->statusBar->showMessage(QString("(") + QString::number(out.x()) + "," +
                                                    QString::number(out.y()) + "," +
                                                    QString::number(out.z()) + ")");
}

void MainWindow::UpdateObjectTree(QVector<CEdge*> edges){

    QTreeWidget* list = this->ui->ObjectList;
    if(list->topLevelItemCount() != edges.size()+1){
        list->clear();

        //原点を追加
        list->addTopLevelItem(new QTreeWidgetItem());
        list->topLevelItem(list->topLevelItemCount()-1)->setText(0,"Origin");
        list->topLevelItem(list->topLevelItemCount()-1)->setIcon(0,QIcon(":/ToolImages/Dot.png"));

        for(int i=0;i<edges.size();i++){
            std::pair<std::string,std::string> p;
            if(edges[i]->is<CLine>  ())p = std::make_pair("Line"  ,":/ToolImages/Line.png");
            if(edges[i]->is<CArc>   ())p = std::make_pair("Arc"   ,":/ToolImages/Arc.png");
            if(edges[i]->is<CSpline>())p = std::make_pair("Spline",":/ToolImages/Spline.png");

            //オブジェクト追加
            list->addTopLevelItem(new QTreeWidgetItem());
            list->topLevelItem(list->topLevelItemCount()-1)->setText(0,p.first.c_str());
            list->topLevelItem(list->topLevelItemCount()-1)->setIcon(0,QIcon(p.second.c_str()));

            if(!list->topLevelItem(list->topLevelItemCount()-1)->isSelected()){
                //list->topLevelItem(list->topLevelItemCount()-1)->setSelected(edges[i]->isSelected());
            }
        }
    }

    //子の設定
    for(int i=0;i<edges.size();i++){
        if(edges[i]->GetMiddleCount()+2 != list->topLevelItem(i+1)->childCount()){
            for(int j=0;j<edges[i]->GetMiddleCount()+2;j++){
                QTreeWidgetItem* child;
                if(j < list->topLevelItem(i+1)->childCount()){
                    child = list->topLevelItem(i+1)->child(j);
                }else{
                    child = new QTreeWidgetItem();
                }

                child->setIcon(0,QIcon(":/ToolImages/Dot.png"));
                if(j == 0){
                    child->setText(0,"Start");
                }
                else if(j == 1){
                    child->setText(0,"End");
                }
                else{
                    child->setText(0,QString("Middle_") + QString::number(j) + ")");
                }

                if(j >= list->topLevelItem(i+1)->childCount()){
                    if(j==0 || j == edges[i]->GetMiddleCount()+1){
                        list->topLevelItem(i+1)->addChild(child);
                    }else{
                        list->topLevelItem(i+1)->insertChild(1,child);
                    }
                }
            }
        }
    }

}

void MainWindow::UpdateObjectTreeSelected(QVector<CObject*>){
    //選択状態の反映
    QTreeWidget* list = this->ui->ObjectList;
    for(int i=0;i<list->topLevelItemCount();i++){
        if(i == 0){
            //原点
            //list->topLevelItem(0)->setSelected(this->model->origin->isSelected());
        }else{
            //Edge自身
            //list->topLevelItem(i)->setSelected(this->model->GetEdges()[i-1]->isSelected());

            //端点
            //list->topLevelItem(i)->child(0)->setSelected(this->model->GetEdges()[i-1]->start->isSelected());
            //list->topLevelItem(i)->child(1)->setSelected(this->model->GetEdges()[i-1]->end->isSelected());
            for(int j=0;j<this->model->GetEdges()[i-1]->GetMiddleCount() && j<list->topLevelItem(i)->childCount()-2;j++){
                //list->topLevelItem(i)->child(j+2)->setSelected(this->model->GetEdges()[i-1]->GetMiddle(j)->isSelected());
            }
        }
    }
}

void MainWindow::UpdateBlocksTree(QVector<CBlock*> blocks){
    //数が一致しなければ、全て削除し再度代入する
    QListWidget* list = this->ui->BlockList;
    if(list->count() != blocks.size()){
        list->clear();
        for(int i=0;i<blocks.size();i++) {
            list->addItem(new QListWidgetItem("CBox"));
            list->item(list->count()-1)->setIcon(QIcon(":/ToolImages/Blocks.png"));
            //list->item(list->count()-1)->setSelected(selecting_block == i);
        }
    }
}

/*
void MainWindow::UpdateBLocksTreeSelected(){
}*/
