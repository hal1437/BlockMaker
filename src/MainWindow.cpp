#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMouseTracking(true);
    //connect(ui->actionCtrlZ          ,SIGNAL(triggered())                        ,this       ,SLOT(CtrlZ()));
    connect(ui->actionDelete         ,SIGNAL(triggered())                        ,this       ,SLOT(Delete()));
    connect(ui->actionEsc            ,SIGNAL(triggered())                        ,this       ,SLOT(Escape()));
    connect(ui->actionMove           ,SIGNAL(triggered())                        ,this       ,SLOT(MoveTransform()));
    connect(ui->actionResetExpantion ,SIGNAL(triggered())                        ,this       ,SLOT(ResetAllExpantion()));
    connect(ui->ToolDimension        ,SIGNAL(triggered())                        ,ui->CadEdit,SLOT(MakeSmartDimension()));
    connect(ui->ToolBlocks           ,SIGNAL(triggered())                        ,this       ,SLOT(MakeBlock()));
    connect(ui->BlockList            ,SIGNAL(itemDoubleClicked(QListWidgetItem*)),ui->CadEdit,SLOT(ConfigureBlock(QListWidgetItem*)));
    connect(ui->CadEdit              ,SIGNAL(ToggleConflict(bool))               ,this       ,SLOT(ToggleConflict(bool)));
    connect(ui->ExportButton         ,SIGNAL(pressed())                          ,ui->CadEdit,SLOT(Export()));

    //CadEditFoam関連
    connect(this        ,SIGNAL(ToggleChanged(CEnum)),this->ui->CadEdit  ,SLOT(SetState(CEnum)));
    connect(ui->CadEdit ,SIGNAL(ScaleChanged(double)),this->ui->ScaleSpin,SLOT(setValue(double)));
    connect(ui->CadEdit ,SIGNAL(RquireRefreshUI())   ,this,SLOT(RefreshUI()));

    //リスト変更系
    connect(ui->RestraintList        ,SIGNAL(itemSelectionChanged())      ,this       ,SLOT(MakeRestraint()));
    connect(ui->ObjectList           ,SIGNAL(itemSelectionChanged())      ,this       ,SLOT(ReciveObjectListChanged()));
    connect(ui->BlockList            ,SIGNAL(itemSelectionChanged())      ,this       ,SLOT(ReciveBlockListChanged ()));

    //CadEditFoamにイベントフィルター導入
    this->installEventFilter(ui->CadEdit);
    RefreshUI();
    ConnectSignals();
    ui->ToolBlocks->setEnabled(false);
    ui->ObjectList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    CObject::drawing_scale = 1.0;

}

MainWindow::~MainWindow()
{
    delete ui;
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
    for(int i =0;i<CObject::selected.size();i++){
        ui->CadEdit->RemoveObject(CObject::selected[i]);
    }
    CObject::selected.clear();
    repaint();
    RefreshUI();
}
void MainWindow::Escape(){
    /*
    if(CObject::createing != nullptr){
        ui->CadEdit->RemoveObject(make_obj);
    }
    creating_count=0;
    ClearButton();
    RefreshUI();

    */
}

void MainWindow::ConnectSignals(){
    connect(ui->ToolDot   ,SIGNAL(toggled(bool)),this,SLOT(ToggledDot(bool)));
    connect(ui->ToolArc   ,SIGNAL(toggled(bool)),this,SLOT(ToggledArc(bool)));
    connect(ui->ToolLine  ,SIGNAL(toggled(bool)),this,SLOT(ToggledLine(bool)));
    connect(ui->ToolRect  ,SIGNAL(toggled(bool)),this,SLOT(ToggledRect(bool)));
    connect(ui->ToolSpline,SIGNAL(toggled(bool)),this,SLOT(ToggledSpline(bool)));
}

void MainWindow::DisconnectSignals(){
    disconnect(ui->ToolDot   ,SIGNAL(toggled(bool)),this,SLOT(ToggledDot(bool)));
    disconnect(ui->ToolArc   ,SIGNAL(toggled(bool)),this,SLOT(ToggledArc(bool)));
    disconnect(ui->ToolLine  ,SIGNAL(toggled(bool)),this,SLOT(ToggledLine(bool)));
    disconnect(ui->ToolRect  ,SIGNAL(toggled(bool)),this,SLOT(ToggledRect(bool)));
    disconnect(ui->ToolSpline,SIGNAL(toggled(bool)),this,SLOT(ToggledSpline(bool)));
}

void MainWindow::ClearButton(){
    //if(make_obj != nullptr && make_obj->isCreating())make_obj->Create(nullptr,-1);
    if(ui->ToolDot   ->isChecked())ui->ToolDot   ->setChecked(false);
    if(ui->ToolLine  ->isChecked())ui->ToolLine  ->setChecked(false);
    if(ui->ToolArc   ->isChecked())ui->ToolArc   ->setChecked(false);
    if(ui->ToolRect  ->isChecked())ui->ToolRect  ->setChecked(false);
    if(ui->ToolSpline->isChecked())ui->ToolSpline->setChecked(false);
}
void MainWindow::RefreshUI(){

    ui->RestraintList->clear();
    //ui->BlockList->clear();

    QVector<RestraintType> able = Restraint::Restraintable(CObject::selected);
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
    this->ui->CadEdit->DrawObjectList(this->ui->ObjectList);
    this->ui->CadEdit->DrawCBoxList  (this->ui->BlockList);

    //拘束更新
    ui->CadEdit->RefreshRestraints();
    //ブロック生成可否判定
    ui->ToolBlocks->setEnabled(CBlock::Creatable(CObject::selected));
    //スマート寸法は1つから
    ui->ToolDimension->setEnabled(CObject::selected.size() >= 1);
    //リスト要素数で出力ボタンの無効化を決定
    ui->ExportButton->setEnabled(this->ui->BlockList->count() > 0);


    this->repaint();
}

#define ToggledToolDefinition(TYPE)             \
void MainWindow::Toggled##TYPE (bool checked){  \
    if(TYPE != Edit)DisconnectSignals();        \
    if(checked){                                \
        ClearButton();                          \
        emit ToggleChanged(TYPE);               \
        ui->Tool##TYPE->setChecked(true);       \
    }else{                                      \
        emit ToggleChanged(Edit);               \
    }                                           \
    ConnectSignals();                           \
}                                               \

ToggledToolDefinition(Dot)
ToggledToolDefinition(Line)
ToggledToolDefinition(Arc)
ToggledToolDefinition(Rect)
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

void MainWindow::MoveTransform(){
    MoveTransformDialog* diag = new MoveTransformDialog(this);
    connect(diag,SIGNAL(RepaintRequest()),this,SLOT(repaint()));
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
    }
}

void MainWindow::MakeBlock(){
    this->ui->CadEdit->MakeBlock();
    RefreshUI();
}


void MainWindow::ReciveObjectListChanged(){
    this->ui->CadEdit->ApplyObjectList(this->ui->ObjectList);
    RefreshUI();
}
void MainWindow::ReciveBlockListChanged(){
    this->ui->CadEdit->ApplyCBoxList  (this->ui->BlockList);
    RefreshUI();
}
void MainWindow::RefreshStatusBar(QMouseEvent*,CObject* under){
    Pos out;
    if(under == nullptr){
        //マウス位置のローカル座標
        out = CObject::mouse_pos;
    }else{
        //選択オブジェクトの最近点
        out = under->GetNear(CObject::mouse_pos);
    }
    this->ui->statusBar->showMessage(QString("(") + QString::number(out.x) + "," + QString::number(out.y) + ")");

}
