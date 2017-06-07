#include "SolidEditForm.h"
#include "ui_SolidEditForm.h"


void SolidEditForm::MakeObject(){
    CObject* hanged = this->GetHangedObject();
    if(state == MAKE_OBJECT::Edit){
        //シフト状態
        if(!shift_pressed)this->model->GetSelected().clear();//選択解除

        //選択状態をトグル
        if(exist(this->model->GetSelected(),hanged))this->model->RemoveSelected(hanged);
        else if(hanged != nullptr)this->model->AddSelected(hanged);
    }else{
        //生成
        this->make_controller->Making(state,this->mouse_pos,hanged);
        //最終点を保持
        this->controller->hang_point = this->make_controller->GetLastPos();
    }
    this->repaint();
}


void SolidEditForm::StartSketch(CFace* face){
    if(this->isSketcheing())return;
    this->sketch_face = face;
    Pos cross = face->GetNorm();
    cross = cross.GetNormalize();
    qDebug() << cross.x() << cross.y() << cross.z();

    double theta1_ = std::atan2(cross.y(),std::sqrt(cross.x()*cross.x()+cross.z()*cross.z()));
    double theta2_ = std::atan2(-cross.x(),cross.z());

    TimeDivider *p1,*p2;
    //カメラ向き占有
    p1 = TimeDivider::TimeDivide(this->theta1,theta1_,500);
    p2 = TimeDivider::TimeDivide(this->theta2,theta2_,500);
    connect(p1,SIGNAL(PerTime()),this,SLOT(repaint()));
    connect(p2,SIGNAL(PerTime()),this,SLOT(repaint()));
}

bool SolidEditForm::isSketcheing(){
    return (this->sketch_face != nullptr);
}

CFace* SolidEditForm::GetHangedFace(){
    Pos  hang_center = (Pos(0,0,1) + this->screen_pos).Dot(Quat::getRotateXMatrix(theta1).Dot(Quat::getRotateYMatrix(theta2)));
    return  this->controller->getHangedFace(hang_center,(this->camera - this->center).GetNormalize());
}
CObject* SolidEditForm::GetHangedObject(){
    Pos  hang_center = (Pos(0,0,1) + this->screen_pos).Dot(Quat::getRotateXMatrix(theta1).Dot(Quat::getRotateYMatrix(theta2)));
    return this->controller->getHangedObject(hang_center,(this->camera - this->center).GetNormalize());
}
void SolidEditForm::ColorSelect(CObject* obj){
    CObject* hang = this->GetHangedObject();
    if     (hang == obj                          )glColor3f(1,0,0);
    else if(exist(this->model->GetSelected(),obj))glColor3f(0,1,1);
    else                                          glColor3f(0,0,1);
}


//カメラ方向セット
void SolidEditForm::setCameraRotate(double theta1,double theta2){
    TimeDivider *p1,*p2;
    p1 = TimeDivider::TimeDivide(this->theta1  ,theta1,500);
    p2 = TimeDivider::TimeDivide(this->theta2  ,theta2,500);
    connect(p1,SIGNAL(PerTime()),this,SLOT(repaint()));
    connect(p2,SIGNAL(PerTime()),this,SLOT(repaint()));
    repaint();
}

void SolidEditForm::keyPressEvent    (QKeyEvent *event){
    //キーイベント
    if(event->key() == Qt::Key_Up    )setCameraRotate(M_PI/2,0);
    if(event->key() == Qt::Key_Left  )setCameraRotate(0,0);
    if(event->key() == Qt::Key_Right )setCameraRotate(0,-M_PI/2);
    if(event->key() == Qt::Key_Down  )setCameraRotate(M_PI/4,-M_PI/4);
    shift_pressed = event->modifiers() & Qt::ShiftModifier;
    ctrl_pressed  = event->modifiers() & Qt::ControlModifier;

    //スケッチ終了
    if(event->key() == Qt::Key_Escape){
        this->controller->hang_point = nullptr;
        this->make_controller->Escape();
        this->sketch_face = nullptr; //スケッチ終了
    }

    this->repaint();
}
void SolidEditForm::keyReleaseEvent  (QKeyEvent *event){
    shift_pressed = event->modifiers() & Qt::ShiftModifier;
    ctrl_pressed  = event->modifiers() & Qt::ControlModifier;
}
void SolidEditForm::mousePressEvent  (QMouseEvent *event){
    this->drag_base   = Pos(event->pos().x(),event->pos().y());
    this->first_click = Pos(event->pos().x(),event->pos().y());

    //操作
    if(this->state != MAKE_OBJECT::Edit && this->isSketcheing()){
        MakeObject();
    }
    if(this->state == MAKE_OBJECT::Edit){
        CObject* hang = this->GetHangedObject();
        if(hang == nullptr){
            this->model->GetSelected().clear();
        }else{
            //単一選択
            if(!this->shift_pressed){
                this->model->GetSelected().clear();
            }
            this->model->AddSelected(hang);
        }
    }
}
void SolidEditForm::mouseReleaseEvent(QMouseEvent *event){
    //ドラッグでなければ
    if(this->first_click == Pos(event->pos().x(),event->pos().y())){
        CFace* f = this->GetHangedFace();
        if(f != nullptr){
            //スケッチ開始
            StartSketch(f);
        }
    }
    this->drag_base = Pos(0,0);
}
void SolidEditForm::mouseMoveEvent   (QMouseEvent *event){

    //スクリーン位置を取得
    this->screen_pos =  Pos(event->pos().x() - this->width()/2,-(event->pos().y() - this->height()/2)) * 2 * round;
    if(this->isSketcheing()){
        //スケッチ中であれば
        Pos Line_base1 = this->screen_pos.Dot(Quat::getRotateXMatrix(theta1).Dot(Quat::getRotateYMatrix(theta2)));
        Pos Line_base2 = Pos(0,0,1)      .Dot(Quat::getRotateXMatrix(theta1).Dot(Quat::getRotateYMatrix(theta2)));
        this->mouse_pos  =  Collision::GetHitPosFaceToLine(this->sketch_face->GetNorm(),*this->sketch_face->corner[0],
                                                           Line_base1,Line_base2);
    }else{
        //カメラ角度から算出
        this->mouse_pos  =  this->screen_pos.Dot(Quat::getRotateXMatrix(theta1).Dot(Quat::getRotateYMatrix(theta2)));
    }
    if(this->drag_base != Pos(0,0) && !this->isSketcheing()){
        //カメラ角度変更
        this->theta1 += static_cast<double>(event->pos().y() - this->drag_base.y())/SENSITIVITY;
        this->theta2 += static_cast<double>(event->pos().x() - this->drag_base.x())/SENSITIVITY;
        this->drag_base = Pos(event->pos().x(),event->pos().y());
    }

    //最終保持座標を更新
    if(this->controller->hang_point != nullptr){
        this->controller->hang_point->Move(this->mouse_pos-*this->controller->hang_point);
    }
    repaint();
}
void SolidEditForm::wheelEvent(QWheelEvent *event){
    this->round += static_cast<double>(event->angleDelta().y())/MOUSE_ZOOM_RATE;
    repaint();
}

void SolidEditForm::SetModel(CadModelCore* model){
    this->model = model;
    this->controller->setModel(model);
    this->make_controller->SetModel(model);
    connect(this->model,SIGNAL(UpdateAnyAction()),this,SLOT(repaint()));
}

void SolidEditForm::initializeGL(){
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    //glutInitDisplayMode(GLUT_RGBA| GLUT_DOUBLE | GLUT_DEPTH);
}

void SolidEditForm::resizeGL(int w, int h){
    glClearColor(0.7,0.7,0.7,1);

    glMatrixMode(GL_PROJECTION);  //行列モード切替
    glLoadIdentity();
    glOrtho(-w,w,
            -h,h,
            -100000000,100000000);

    glMatrixMode(GL_MODELVIEW); //行列モードを戻す
    glLoadIdentity();
    gluLookAt(camera.x(), camera.y(), camera.z(),
              center.x(), center.y(), center.z(),
              0, 1, 0);
}

void SolidEditForm::paintGL(){

    //カメラ調整
    if(theta1 >  M_PI/2) theta1 =  M_PI/2;
    if(theta1 < -M_PI/2) theta1 = -M_PI/2;
    this->camera = Pos(0,0,round).Dot(Quat::getRotateXMatrix(theta1).Dot(Quat::getRotateYMatrix(theta2)));
    glMatrixMode(GL_PROJECTION);  //行列モード切替
    glLoadIdentity();
    glOrtho(-this->width() *(round),
             this->width() *(round),
            -this->height()*(round),
             this->height()*(round),-10000,10000);
    glMatrixMode(GL_MODELVIEW); //行列モードを戻す
    glLoadIdentity();
    gluLookAt(camera.x(), camera.y(), camera.z(),
              center.x(), center.y(), center.z(),
                       0,          1,          0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.7,0.7,0.7,1);//背景

    //描画面と色のリスト
    QVector<std::pair<CFace*,QVector<int>>> faces;
    faces.push_back(std::make_pair(controller->getSideFace() ,QVector<int>({1,0,0})));//右側面
    faces.push_back(std::make_pair(controller->getTopFace()  ,QVector<int>({0,1,0})));//平面
    faces.push_back(std::make_pair(controller->getFrontFace(),QVector<int>({0,0,1})));//正面
    faces.push_back(std::make_pair(this->sketch_face         ,QVector<int>({1,1,0})));//スケッチ面
    if(this->drag_base == Pos(0,0) && !this->isSketcheing()){
        faces.push_back(std::make_pair(this->GetHangedFace()     ,QVector<int>({0,1,1})));//直下面
    }
    //面の描画
    glLineWidth(2);
    for(std::pair<CFace*,QVector<int>>f: faces){
        if(f.first != nullptr){
            glColor3f(f.second[0], f.second[1], f.second[2]);
            f.first->DrawGL(this->camera,this->center);
        }
    }

    //中央線の描画
    for(int i=0;i<3;i++){
        glBegin(GL_LINES);
        glColor3f((i==0), (i==1), (i==2));
        glVertex3f(0,0,0);
        glVertex3f(50*(i==0), 50*(i==1), 50*(i==2));
        glEnd();
    }

    //エッジ描画
    for(int i=0;i<this->model->GetEdges().size();i++){
        CEdge* edge = this->model->GetEdges()[i];
        //描画
        edge->DrawGL(this->camera,this->center);

        //端点の描画
        for(int j=0;j<edge->GetPosSequenceCount();j++){
            edge->GetPosSequence(j)->DrawGL(this->camera,this->center);
        }
    }
    //面描画
    for(int i=0;i<this->model->GetFaces().size();i++){
        CFace* edge = this->model->GetFaces()[i];
        //描画
        edge->DrawGL(this->camera,this->center);
    }

    //選択オブジェクト描画
    glColor3f(0,1,1);
    for(CObject* p: this->model->GetSelected()){
        p->DrawGL(this->camera,this->center);
    }

    //直下オブジェクトの描画
    glColor3f(1,0,0);
    CObject* hang_obj = this->GetHangedObject();
    if(hang_obj != nullptr){
        hang_obj->DrawGL(this->camera,this->center);
    }

    //ブロック描画
    for(int i=0;i<this->model->GetBlocks().size();i++){
        //CBlock* block = this->model->GetBlocks()[i];

        for(int j=0;j<4;j++){
            glBegin(GL_LINE_LOOP);
            glColor3f(0,0,0);
            //glVertex3f(block->GetEdge(j)->start,block->GetEdge(j)->start,block->GetEdge(j)->start);
            glEnd();
        }
    }

    glFlush();
}


void SolidEditForm::SetState(MAKE_OBJECT state){
    this->state = state;
}
void SolidEditForm::CEdgeChanged(QVector<CEdge*>){
    this->repaint();
}
void SolidEditForm::CBlockChanged(QVector<CBlock*>){
    this->repaint();
}

SolidEditForm::SolidEditForm(QWidget *parent) :
    QOpenGLWidget(parent),
    ui(new Ui::SolidEditForm)
{
    ui->setupUi(this);
    make_controller = new MakeObjectController();
    //マウストラッキング対象
    setMouseTracking(true);

    this->controller = new SolidEditController();
    this->camera = Pos(round,0,0);
    this->center = Pos(0,0,0);
    this->camera.x() = round * std::cos(theta2);
    this->camera.y() = round * std::sin(theta1);
    this->camera.z() = round * std::sin(theta2);
}

SolidEditForm::~SolidEditForm()
{
    delete ui;
    delete this->controller;
}
