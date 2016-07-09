#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMouseTracking(true);
    connect(ui->CadEdit        ,SIGNAL(MovedMouse(QMouseEvent*,CObject*)),this       ,SLOT(MovedMouse(QMouseEvent*,CObject*)));
    connect(ui->actionCtrlZ    ,SIGNAL(triggered())                      ,this       ,SLOT(CtrlZ()));
    connect(ui->actionDelete   ,SIGNAL(triggered())                      ,this       ,SLOT(Delete()));
    connect(ui->actionEsc      ,SIGNAL(triggered())                      ,this       ,SLOT(Escape()));
    connect(ui->RestraintList  ,SIGNAL(itemClicked(QListWidgetItem*))    ,this       ,SLOT(MakeRestraint(QListWidgetItem*)));
    connect(ui->SizeRateSpinBox,SIGNAL(valueChanged(double))             ,ui->CadEdit,SLOT(SetScale(double)));
    connect(ui->ToolDimension  ,SIGNAL(triggered())                      ,ui->CadEdit,SLOT(MakeSmartDimension()));
    connect(ui->ToolBlocks     ,SIGNAL(triggered())                      ,this       ,SLOT(MakeBlock()));
    ConnectSignals();
    ui->ToolBlocks->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent  (QMouseEvent*){
    release_flag=false;
    if(state==Edit)move_flag = true;
    MakeObject();
    RefreshRestraintList();
}

void MainWindow::mouseReleaseEvent(QMouseEvent*){
    //編集
    move_flag = false;
    if(release_flag==true)MakeObject();
}
void MainWindow::wheelEvent(QWheelEvent * e){
    if(ctrl_pressed){
        double value = ui->CadEdit->GetScale() + (e->angleDelta().y() / 256.0f);
        ui->SizeRateSpinBox->setValue(value);
        ui->CadEdit->SetScale(value);
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

void MainWindow::CtrlZ(){
    if(!log.empty()){
        ui->CadEdit->RemoveObject(log.back());
        log.erase(log.end()-1);
        creating_count=0;
    }
}
void MainWindow::Delete(){
    ui->CadEdit->RemoveObject(CObject::selecting);
}
void MainWindow::Escape(){
    if(make_obj != nullptr && !make_obj->Make(nullptr,-1)){
        ui->CadEdit->RemoveObject(make_obj);
    }
    creating_count=0;
    ClearButton();
}

void MainWindow::MovedMouse(QMouseEvent *event, CObject *under_object){
    static Pos past;
    if(!(event->buttons() & Qt::LeftButton) || !move_flag){
        CObject::selecting = under_object;
    }
    //編集
    if(move_flag == true){
        if(CObject::selecting!=nullptr && !CObject::selecting->isLocked()){
            CObject::selecting->Move(CObject::mouse_over-past);
        }
    }

    if(CBlocks::Creatable(CObject::selected)){
        ui->ToolBlocks->setEnabled(true);
    }
    past = CObject::mouse_over;
    release_flag=true;

    ui->CadEdit->RefreshRestraints();
    repaint();
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
    if(make_obj != nullptr && make_obj->isCreateing())make_obj->Make(nullptr,-1);
    if(ui->ToolDot   ->isChecked())ui->ToolDot   ->setChecked(false);
    if(ui->ToolLine  ->isChecked())ui->ToolLine  ->setChecked(false);
    if(ui->ToolArc   ->isChecked())ui->ToolArc   ->setChecked(false);
    if(ui->ToolRect  ->isChecked())ui->ToolRect  ->setChecked(false);
    if(ui->ToolSpline->isChecked())ui->ToolSpline->setChecked(false);
}
void MainWindow::RefreshRestraintList(){
    ui->RestraintList->clear();
    std::vector<RestraintType> able = Restraint::Restraintable(CObject::selected);
    for(RestraintType r:able){
        std::pair<std::string,std::string> p;
        if(r == MATCH     )p = std::make_pair("一致",":/Restraint/MatchRestraint.png");
        if(r == EQUAL     )p = std::make_pair("等値",":/Restraint/EqualRestraint.png");
        if(r == CONCURRENT)p = std::make_pair("並行",":/Restraint/ConcurrentRestraint.png");
        if(r == VERTICAL  )p = std::make_pair("垂直",":/Restraint/VerticalRestraint.png");
        if(r == HORIZONTAL)p = std::make_pair("水平",":/Restraint/HorizontalRestraint.png");
        if(r == TANGENT   )p = std::make_pair("正接",":/Restraint/TangentRestraint.png");
        if(r == FIX       )p = std::make_pair("固定",":/Restraint/FixRestraint.png");
        if(r == MARGE     )p = std::make_pair("マージ",":/Restraint/Marge.png");
        ui->RestraintList->addItem(new QListWidgetItem(p.first.c_str()));
        ui->RestraintList->item(ui->RestraintList->count()-1)->setIcon(QIcon(p.second.c_str()));
    }
}

#define ToggledToolDefinition(TYPE)             \
void MainWindow::Toggled##TYPE (bool checked){  \
    if(TYPE != Edit)DisconnectSignals();        \
    if(checked){                                \
        ClearButton();                          \
        state = TYPE;                           \
        ui->Tool##TYPE->setChecked(true);       \
    }else{                                      \
        if(make_obj != nullptr && make_obj->isCreateing()){\
            make_obj->Make(nullptr,-1);           \
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

void MainWindow::Export(){
    ExportDialog* diag = new ExportDialog(this);
    diag->SetBlocks(blocks);
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
    if(ui->RestraintList->currentItem()->text() == "固定")type = FIX;
    if(type != Paradox)ui->CadEdit->MakeRestraint(type);

    if(ui->RestraintList->currentItem()->text() == "マージ"){
    //    ui->CadEdit->
    }
}

void MainWindow::MakeObject(){

    Pos local_pos = CObject::mouse_over;
    if(CObject::selecting != nullptr)local_pos = CObject::selecting->GetNear(local_pos);

    release_flag=false;

    //並行移動

    //編集
    if(state == Edit){
        //シフト状態
        if(!shift_pressed)CObject::selected.clear();

        //トグル化
        if(exist(CObject::selected,CObject::selecting))erase(CObject::selected,CObject::selecting);
        else if(CObject::selecting != nullptr)CObject::selected.push_back(CObject::selecting);

        //スマート寸法は1つから
        ui->ToolDimension->setEnabled(CObject::selected.size() >= 1);
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
        }
        //作成
        if(MakeJoint(make_obj)){

            //未構築点を追加
            ui->CadEdit->CompleteObject(make_obj);

            //子オブジェクトを追加
            std::vector<CObject*> cc = make_obj->GetChild();
            for(CObject* c:cc)ui->CadEdit->AddObject(c);

            creating_count = 0;
        }else {
            creating_count++;
        }
    }
    ui->CadEdit->RefreshRestraints();
    repaint();
}

bool MainWindow::MakeJoint(CObject* obj){
    Pos local_pos = CObject::mouse_over;
    if(CObject::selecting != nullptr)local_pos = CObject::selecting->GetNear(local_pos);

    //端点に点を作成
    if(CObject::selecting == nullptr){
        //端点に点を作成
        CPoint* new_point = new CPoint(CObject::mouse_over);
        new_point->Make(new_point);
        ui->CadEdit->AddObject(new_point);
        log.push_back(new_point);
        return obj->Make(new_point,creating_count);
    }else if(CObject::selecting->is<CPoint>()){
        //点をマージ
        return obj->Make(dynamic_cast<CPoint*>(CObject::selecting),creating_count);
    }else{
        //点をオブジェクト上に追加
        CPoint* new_point = new CPoint(CObject::selecting->GetNear(CObject::mouse_over));
        new_point->Make(new_point);
        ui->CadEdit->AddObject(new_point);
        log.push_back(new_point);

        //一致の幾何拘束を付与
        return  obj->Make(new_point,creating_count);
    }
}
bool MainWindow::MakeBlock(){
    CBoxDefineDialog* diag = new CBoxDefineDialog();
    if(diag->exec()){
        CBlocks block = diag->ExportCBlocks();
        block.SetNode(CObject::selected);
        this->blocks.push_back(block);
        CObject::selected.clear();
    }
}

