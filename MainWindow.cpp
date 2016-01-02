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
    connect(ui->SizeRateSpinBox,SIGNAL(valueChanged(double))             ,ui->CadEdit,SLOT(SetScale(double)));
    connect(ui->ToolDimension  ,SIGNAL(triggered())                      ,ui->CadEdit,SLOT(MakeSmartDimension()));
    ConnectSignals();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent  (QMouseEvent* event){
    Pos local_pos = CObject::mouse_over;
    if(CObject::selecting != nullptr)local_pos = CObject::selecting->GetNear(local_pos);

    release_flag=false;


    //並行移動
    //if(event->buttons() == )

    //編集
    if(state == Edit){
        move_flag = true;
    }

    //点の追加
    if(state == Dot){
        make_obj = new CPoint();
        make_obj->Make(Pos(local_pos.x,local_pos.y));
        ui->CadEdit->AddObject(make_obj);
        log.push_back(make_obj);
    }
    //線の追加
    if(state == Line){
        //新規作成
        if(creating_count == 0){
            make_obj = new CLine();
            ui->CadEdit->AddObject(make_obj);

            make_obj->Make(CObject::mouse_over,creating_count);
            log.push_back(make_obj);
        }

        if(CObject::selecting == nullptr){
            //端点に点を作成
            CPoint* new_point = new CPoint();
            new_point->Make(Pos(local_pos.x,local_pos.y));
            ui->CadEdit->AddObject(new_point);
            log.push_back(new_point);
            make_obj->Make(*new_point,creating_count);
        }else if(CObject::selecting->is<CPoint>()){
            //点と結合
            make_obj->Make(*dynamic_cast<CPoint*>(CObject::selecting),creating_count);
        }else if(CObject::selecting->is<CLine>()){
            //点を線上に追加
            CPoint* new_point = new CPoint();
            new_point->Make(Pos(local_pos.x,local_pos.y));
            ui->CadEdit->AddObject(new_point);
            log.push_back(new_point);
            make_obj->Make(*new_point,creating_count);
        }
        creating_count++;
        creating_count %= 2;
    }
    //円弧の追加
    if(state == Arc){
        //新規作成
        if(creating_count == 0){
            make_obj = new CArc();
            ui->CadEdit->AddObject(make_obj);

            make_obj->Make(CObject::mouse_over,creating_count);
            log.push_back(make_obj);
        }

        if(CObject::selecting == nullptr){
            //端点に点を作成
            CPoint* new_point = new CPoint();
            new_point->Make(Pos(local_pos.x,local_pos.y));
            ui->CadEdit->AddObject(new_point);
            log.push_back(new_point);
            make_obj->Make(*new_point,creating_count);
        }else if(CObject::selecting->is<CPoint>()){
            //点と結合
            make_obj->Make(*dynamic_cast<CPoint*>(CObject::selecting),creating_count);
        }else if(CObject::selecting->is<CLine>()){
            //点を線上に追加
            CPoint* new_point = new CPoint();
            new_point->Make(Pos(local_pos.x,local_pos.y));
            ui->CadEdit->AddObject(new_point);
            log.push_back(new_point);
            make_obj->Make(*new_point,creating_count);
        }
        creating_count++;
        creating_count %= 2;
    }
    repaint();
}

void MainWindow::mouseReleaseEvent(QMouseEvent* ){
    Pos local_pos = CObject::mouse_over;
    if(CObject::selecting != nullptr)local_pos = CObject::selecting->GetNear(local_pos);


    //編集
    if(state == Edit){
        move_flag = false;
        if(!shift_pressed)CObject::selected.clear();
        if(exist(CObject::selected,CObject::selecting))erase(CObject::selected,CObject::selecting);
        else if(CObject::selecting != nullptr)CObject::selected.push_back(CObject::selecting);
        ui->ToolDimension->setEnabled(CObject::selected.size() == 2);
    }

    if(state == Line && release_flag==true){
        if(creating_count == 0){
            ui->CadEdit->AddObject(make_obj);
            log.push_back(make_obj);
        }

        //端点に点を作成
        if(CObject::selecting == nullptr){
            //端点に点を作成
            CPoint* new_point = new CPoint();
            new_point->Make(Pos(local_pos.x,local_pos.y));
            ui->CadEdit->AddObject(new_point);
            log.push_back(new_point);
            make_obj->Make(*new_point,creating_count);
        }else if(CObject::selecting->is<CPoint>()){
            //点と結合
            make_obj->Make(*dynamic_cast<CPoint*>(CObject::selecting),creating_count);
        }else if(CObject::selecting->is<CLine>()){
            //点を線上に追加
            CPoint* new_point = new CPoint();
            new_point->Make(Pos(local_pos.x,local_pos.y));
            ui->CadEdit->AddObject(new_point);
            log.push_back(new_point);
            make_obj->Make(*new_point,creating_count);
        }
        creating_count++;
        creating_count %= 2;
    }
    repaint();
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
    connect(ui->ToolDot ,SIGNAL(toggled(bool)),this,SLOT(ToggledDot(bool)));
    connect(ui->ToolArc ,SIGNAL(toggled(bool)),this,SLOT(ToggledArc(bool)));
    connect(ui->ToolLine,SIGNAL(toggled(bool)),this,SLOT(ToggledLine(bool)));
}

void MainWindow::DisconnectSignals(){
    disconnect(ui->ToolDot ,SIGNAL(toggled(bool)),this,SLOT(ToggledDot(bool)));
    disconnect(ui->ToolArc ,SIGNAL(toggled(bool)),this,SLOT(ToggledArc(bool)));
    disconnect(ui->ToolLine,SIGNAL(toggled(bool)),this,SLOT(ToggledLine(bool)));
}

void MainWindow::ClearButton(){
    if(ui->ToolDot ->isChecked())ui->ToolDot->setChecked(false);
    if(ui->ToolLine->isChecked())ui->ToolLine->setChecked(false);
    if(ui->ToolArc ->isChecked())ui->ToolArc ->setChecked(false);
}

void MainWindow::ToggledDot (bool checked){

    DisconnectSignals();
    if(checked){
        ClearButton();
        //選択された
        state = Dot;
        ui->ToolDot->setChecked(true);
    }else{
        //解除された
        state = Edit;
    }
    ConnectSignals();
}

void MainWindow::ToggledLine(bool checked){
    DisconnectSignals();
    ClearButton();
    if(checked){
        //選択された
        state = Line;
        ui->ToolLine->setChecked(true);
    }else{
        //解除された
        state = Edit;
    }
    ConnectSignals();
}

void MainWindow::ToggledArc (bool checked){
    DisconnectSignals();
    ClearButton();
    if(checked){
        //選択された
        state = Arc;
        ui->ToolArc->setChecked(true);
    }else{
        //解除された
        state = Edit;
    }
    ConnectSignals();
}


