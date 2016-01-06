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
    connect(ui->SizeRateSpinBox,SIGNAL(valueChanged(double))             ,ui->CadEdit,SLOT(SetScale(double)));
    connect(ui->ToolDimension  ,SIGNAL(triggered())                      ,ui->CadEdit,SLOT(MakeSmartDimension()));
    ConnectSignals();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent  (QMouseEvent*){
    release_flag=false;
    move_flag = true;
    MakeObject();
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
    if(!make_obj->Make(Pos(),-1)){
        ui->CadEdit->RemoveObject(make_obj);
    }
}

void MainWindow::MovedMouse(QMouseEvent *event, CObject *under_object){
    static Pos past;
    if(!(event->buttons() & Qt::LeftButton) || !move_flag){

        CObject::selecting = under_object;
    }
    //編集
    if(move_flag == true){
        if(CObject::selecting!=nullptr)CObject::selecting->Move(CObject::mouse_over-past);
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
    connect(ui->ToolSpline,SIGNAL(toggled(bool)),this,SLOT(ToggledSpline(bool)));
}

void MainWindow::DisconnectSignals(){
    disconnect(ui->ToolDot   ,SIGNAL(toggled(bool)),this,SLOT(ToggledDot(bool)));
    disconnect(ui->ToolArc   ,SIGNAL(toggled(bool)),this,SLOT(ToggledArc(bool)));
    disconnect(ui->ToolLine  ,SIGNAL(toggled(bool)),this,SLOT(ToggledLine(bool)));
    disconnect(ui->ToolSpline,SIGNAL(toggled(bool)),this,SLOT(ToggledSpline(bool)));
}

void MainWindow::ClearButton(){
    if(ui->ToolDot   ->isChecked())ui->ToolDot   ->setChecked(false);
    if(ui->ToolLine  ->isChecked())ui->ToolLine  ->setChecked(false);
    if(ui->ToolArc   ->isChecked())ui->ToolArc   ->setChecked(false);
    if(ui->ToolSpline->isChecked())ui->ToolSpline->setChecked(false);
}

#define ToggledToolDefinition(TYPE)             \
void MainWindow::Toggled##TYPE (bool checked){  \
    if(TYPE != Edit)DisconnectSignals();        \
    if(checked){                                \
        ClearButton();                          \
        state = TYPE;                           \
        ui->Tool##TYPE->setChecked(true);       \
    }else{                                      \
        state = Edit;                           \
    }                                           \
    ConnectSignals();                           \
}                                               \

ToggledToolDefinition(Dot)
ToggledToolDefinition(Line)
ToggledToolDefinition(Arc)
ToggledToolDefinition(Spline)

void MainWindow::MakeObject(){

    Pos local_pos = CObject::mouse_over;
    if(CObject::selecting != nullptr)local_pos = CObject::selecting->GetNear(local_pos);

    release_flag=false;

    //並行移動
    //if(event->buttons() == )

    //編集
    if(state == Edit){
        if(!shift_pressed)CObject::selected.clear();
        if(exist(CObject::selected,CObject::selecting))erase(CObject::selected,CObject::selecting);
        else if(CObject::selecting != nullptr)CObject::selected.push_back(CObject::selecting);
        ui->ToolDimension->setEnabled(CObject::selected.size() == 2);
    }else if(state == Dot){
        //点の追加
        make_obj = new CPoint();
        make_obj->Make(Pos(local_pos.x,local_pos.y));
        ui->CadEdit->AddObject(make_obj);
        log.push_back(make_obj);
    }else{
        //新規作成
        if(creating_count == 0){
            if     (state == Line  )make_obj = new CLine();
            else if(state == Arc   )make_obj = new CArc();
            else if(state == Spline)make_obj = new CSpline();
            ui->CadEdit->AddObject(make_obj);

            make_obj->Make(CObject::mouse_over,creating_count);
            log.push_back(make_obj);
        }

        if(MakeJoint(make_obj)){
            creating_count = 0;
        }else creating_count++;
    }
    repaint();
}

bool MainWindow::MakeJoint(CObject* obj){
    Pos local_pos = CObject::mouse_over;
    if(CObject::selecting != nullptr)local_pos = CObject::selecting->GetNear(local_pos);

    //端点に点を作成
    if(CObject::selecting == nullptr){
        //端点に点を作成
        CPoint* new_point = new CPoint();
        new_point->Make(Pos(local_pos.x,local_pos.y));
        ui->CadEdit->AddObject(new_point);
        log.push_back(new_point);
        return obj->Make(*new_point,creating_count);
    }else if(CObject::selecting->is<CPoint>()){
        //点と結合
        return obj->Make(*dynamic_cast<CPoint*>(CObject::selecting),creating_count);
    }else if(CObject::selecting->is<CLine>()){
        //点を線上に追加
        CPoint* new_point = new CPoint();
        new_point->Make(Pos(local_pos.x,local_pos.y));
        ui->CadEdit->AddObject(new_point);
        log.push_back(new_point);
        return  obj->Make(*new_point,creating_count);
    }
}


