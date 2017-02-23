#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMouseTracking(true);
    connect(ui->CadEdit          ,SIGNAL(MovedMouse(QMouseEvent*,CObject*))  ,this       ,SLOT(MovedMouse(QMouseEvent*,CObject*)));
    connect(ui->actionCtrlZ      ,SIGNAL(triggered())                        ,this       ,SLOT(CtrlZ()));
    connect(ui->actionDelete     ,SIGNAL(triggered())                        ,this       ,SLOT(Delete()));
    connect(ui->actionEsc        ,SIGNAL(triggered())                        ,this       ,SLOT(Escape()));
    connect(ui->RestraintList    ,SIGNAL(itemClicked(QListWidgetItem*))      ,this       ,SLOT(MakeRestraint(QListWidgetItem*)));
    connect(ui->SizeRateSpinBox  ,SIGNAL(valueChanged(double))               ,ui->CadEdit,SLOT(SetScale(double)));
    connect(ui->ToolDimension    ,SIGNAL(triggered())                        ,ui->CadEdit,SLOT(MakeSmartDimension()));
    connect(ui->ToolBlocks       ,SIGNAL(triggered())                        ,ui->CadEdit,SLOT(MakeBlock()));
    connect(ui->ObjectList       ,SIGNAL(itemClicked(QListWidgetItem*))      ,this       ,SLOT(ReciveObjectListChanged(QListWidgetItem*)));
    connect(ui->BlockList        ,SIGNAL(itemClicked(QListWidgetItem*))      ,this       ,SLOT(ReciveBlockListChanged (QListWidgetItem*)));
    connect(ui->BlockList        ,SIGNAL(itemDoubleClicked(QListWidgetItem*)),ui->CadEdit,SLOT(ConfigureBlock(QListWidgetItem*)));
    connect(ui->CadEdit          ,SIGNAL(ToggleConflict(bool))               ,this       ,SLOT(ToggleConflict(bool)));

    ConnectSignals();
    ui->ToolBlocks->setEnabled(false);
    ui->ObjectList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    CObject::drawing_scale = 1.0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent  (QMouseEvent*){
    release_flag = false;
    if(state==Edit)move_flag = true;
    MakeObject();
}

void MainWindow::mouseReleaseEvent(QMouseEvent*){
    //編集
    move_flag = false;
    if(release_flag==true)MakeObject();
}
void MainWindow::wheelEvent(QWheelEvent * e){
    //拡大
    double delta = (e->angleDelta().y())/10000.0;//差分値
    double x = std::log(ui->CadEdit->GetScale());
    double next_scale = std::exp(x + delta);//次の拡大値
    Pos    next_translate = ui->CadEdit->GetTranslate();//次の平行移動値

    //Pos center = CObject::mouse_pos;
    //double rate = (next_scale / ui->CadEdit->GetScale());

    //next_translate = center + (ui->CadEdit->GetTranslate() - center) * rate;
    //拡大値は負にならない
    if(next_scale > 0){
        //適応
        ui->SizeRateSpinBox->setValue(next_scale);
        //ui->CadEdit->SetTranslate(next_translate);
        ui->CadEdit->SetScale(next_scale);
        CObject::drawing_scale = next_scale;
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event){
    shift_pressed = event->modifiers() & Qt::ShiftModifier;
    ctrl_pressed  = event->modifiers() & Qt::ControlModifier;
}
void MainWindow::keyReleaseEvent(QKeyEvent* event){
    shift_pressed = event->modifiers() & Qt::ShiftModifier;
    ctrl_pressed  = event->modifiers() & Qt::ControlModifier;
}

void MainWindow::paintEvent(QPaintEvent*){
}

void MainWindow::CtrlZ(){
    if(!log.empty()){
        ui->CadEdit->RemoveObject(log.back());
        log.erase(log.end()-1);
        creating_count=0;
    }
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
    if(make_obj != nullptr && !make_obj->Create(nullptr,-1)){
        ui->CadEdit->RemoveObject(make_obj);
    }
    creating_count=0;
    ClearButton();
}

void MainWindow::MovedMouse(QMouseEvent *event, CObject *under_object){
    static Pos past;
    static Pos piv;//画面移動ピボット

    //選択
    if(!(event->buttons() & Qt::LeftButton) || !move_flag){
        CObject::hanged = under_object;
        piv = Pos(-1,-1);
    }
    //画面移動
    if((event->buttons() & Qt::LeftButton) && CObject::hanged == nullptr && this->state == Edit){
        if(piv == Pos(-1,-1)){
            piv = (CObject::mouse_pos - this->ui->CadEdit->GetTranslate());
        }
        Pos hand = (CObject::mouse_pos - this->ui->CadEdit->GetTranslate());
        this->ui->CadEdit->SetTranslate((this->ui->CadEdit->GetTranslate() + piv - hand) / this->ui->CadEdit->GetScale());
        piv = hand;
    }

    //編集
    if(move_flag == true){
        if(CObject::hanged != nullptr && !CObject::hanged->isLock()){
            CObject::hanged->Move(CObject::mouse_pos - past);
        }
    }

    //拘束更新
    ui->CadEdit->RefreshRestraints();

    past = CObject::mouse_pos;
    release_flag=true;

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
    if(make_obj != nullptr && make_obj->isCreating())make_obj->Create(nullptr,-1);
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

    this->repaint();
}

#define ToggledToolDefinition(TYPE)             \
void MainWindow::Toggled##TYPE (bool checked){  \
    if(TYPE != Edit)DisconnectSignals();        \
    if(checked){                                \
        ClearButton();                          \
        state = TYPE;                           \
        ui->Tool##TYPE->setChecked(true);       \
    }else{                                      \
        if(make_obj != nullptr && make_obj->isCreating()){\
            make_obj->Create(nullptr,-1);       \
            creating_count=0;                   \
        }                                       \
        make_obj = nullptr;                     \
        state = Edit;                           \
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
void MainWindow::Export(){
    ExportDialog* diag = new ExportDialog(this);
    //diag->SetBlocks(blocks);
    diag->exec();
}

void MainWindow::MakeRestraint(QListWidgetItem *){
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
    //    ui->CadEdit->
    }
}

void MainWindow::MakeObject(){

    Pos local_pos = CObject::mouse_pos;
    if(CObject::hanged != nullptr)local_pos = CObject::hanged->GetNear(local_pos);

    release_flag=false;

    //並行移動

    //編集
    if(state == Edit){
        //シフト状態
        if(!shift_pressed)CObject::selected.clear();

        //選択状態をトグル
        if(exist(CObject::selected,CObject::hanged))erase(CObject::selected,CObject::hanged);
        else if(CObject::hanged != nullptr)CObject::selected.push_back(CObject::hanged);

    }else{
        //新規オブジェクト
        if(creating_count == 0){
            if     (state == Dot   )make_obj = new CPoint();
            else if(state == Line  )make_obj = new CLine();
            else if(state == Arc   )make_obj = new CArc();
            else if(state == Rect  )make_obj = new CRect();
            else if(state == Spline)make_obj = new CSpline();
            ui->CadEdit->AddObject(make_obj);
            log.push_back(make_obj);
            CObject::createing = make_obj;
        }
        //作成
        if(MakeJoint(make_obj) == true){
            //生成完了
            CObject::createing = nullptr;
            creating_count = 0;

            //ジョイントを追加
            for(int i=0;i<make_obj->GetJointNum();i++){
                ui->CadEdit->AddObject(make_obj->GetJoint(i));
            }
        }else {
            //生成継続

            creating_count++;
        }
    }
    ui->CadEdit->RefreshRestraints();
    RefreshUI();
}

bool MainWindow::MakeJoint(CObject* obj){
    Pos local_pos = CObject::mouse_pos;
    if(CObject::hanged != nullptr)local_pos = CObject::hanged->GetNear(local_pos);

    //端点に点を作成
    if(CObject::hanged == nullptr){
        //端点に点を作成
        CPoint* new_point = new CPoint(CObject::mouse_pos);
        new_point->Create(new_point,0);
        log.push_back(new_point);
        return obj->Create(new_point,creating_count);
    }else if(CObject::hanged->is<CPoint>()){
        //点をマージ
        return obj->Create(dynamic_cast<CPoint*>(CObject::hanged),creating_count);
    }else{
        //点をオブジェクト上に追加
        CPoint* new_point = new CPoint(CObject::hanged->GetNear(CObject::mouse_pos));
        new_point->Create(new_point,0);
        log.push_back(new_point);

        //一致の幾何拘束を付与
        return  obj->Create(new_point,creating_count);
    }
}
void MainWindow::ReciveObjectListChanged(QListWidgetItem* current){
    this->ui->CadEdit->ApplyObjectList(this->ui->ObjectList);
    RefreshUI();
}
void MainWindow::ReciveBlockListChanged(QListWidgetItem* current){
    this->ui->CadEdit->ApplyCBoxList(this->ui->BlockList);
    RefreshUI();
}

