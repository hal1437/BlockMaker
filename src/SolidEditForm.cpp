#include "SolidEditForm.h"
#include "ui_SolidEditForm.h"


void SolidEditForm::MakeObject(){
    CObject* hanged = this->GetHangedObject();
    if(state != MAKE_OBJECT::Edit){
        //生成
        this->make_controller->Making(state,this->mouse_pos,hanged);
        //最終点を保持
        this->controller->hang_point = this->make_controller->GetLastPos();
    }
    this->repaint();
}

CFace* SolidEditForm::GetHangedFace(){
    if(this->controller->isSketcheing())return nullptr;
    Pos  hang_center = (Pos(0,0,1) + this->screen_pos).Dot(this->controller->getCameraMatrix());
    return  this->controller->getHangedFace(hang_center,this->camera*this->round*1000 + hang_center);
}
CObject* SolidEditForm::GetHangedObject(){
    Pos  hang_center = (Pos(0,0,1) + this->screen_pos).Dot(this->controller->getCameraMatrix());
    return this->controller->getHangedObject(hang_center,(this->camera - this->center).GetNormalize());
}
void SolidEditForm::ColorSelect(CObject* obj){
    CObject* hang = this->GetHangedObject();
    if     (hang == obj                          )glColor3f(1,0,0);
    else if(exist(this->model->GetSelected(),obj))glColor3f(0,1,1);
    else                                          glColor3f(0,0,1);
}

//カメラ方向セット


void SolidEditForm::keyPressEvent    (QKeyEvent *event){
    //キーイベント
    if(!this->controller->isSketcheing()){
        if(event->key() == Qt::Key_Up    )this->setCameraRotate(M_PI/2,0);
        if(event->key() == Qt::Key_Left  )this->setCameraRotate(0,0);
        if(event->key() == Qt::Key_Right )this->setCameraRotate(0,-M_PI/2);
        if(event->key() == Qt::Key_Down  )this->setCameraRotate(M_PI/4,-M_PI/4);
    }
    shift_pressed = event->modifiers() & Qt::ShiftModifier;
    ctrl_pressed  = event->modifiers() & Qt::ControlModifier;

    //スケッチ終了
    if(event->key() == Qt::Key_Escape){
        this->controller->hang_point = nullptr;
        this->make_controller->Escape();
        this->controller->sketch_face = nullptr; //スケッチ終了
    }

    this->repaint();
}
void SolidEditForm::StartSketch(CFace* face){
    if(this->controller->isSketcheing())return;
    this->model->SelectedClear();//選択解除

    this->controller->sketch_face = face;
    Pos cross = face->GetNorm();
    cross = cross.GetNormalize();

    //法線ベクトルを近い方に変更
    if(cross.DotPos(this->camera - this->center) <  0){
        cross = -cross;
    }

    double theta1_ = std::atan2(cross.y(),std::sqrt(cross.x()*cross.x()+cross.z()*cross.z()));
    double theta2_ = std::atan2(-cross.x(),cross.z());

    this->setCameraRotate(theta1_,theta2_);
}
void SolidEditForm::setCameraRotate(double theta1,double theta2){
    TimeDivider *p1,*p2;
    p1 = TimeDivider::TimeDivide(this->controller->theta1  ,theta1,500);
    p2 = TimeDivider::TimeDivide(this->controller->theta2  ,theta2,500);
    connect(p1,SIGNAL(PerTime()),this,SLOT(repaint()));
    connect(p2,SIGNAL(PerTime()),this,SLOT(repaint()));
}
void SolidEditForm::keyReleaseEvent  (QKeyEvent *event){
    shift_pressed = event->modifiers() & Qt::ShiftModifier;
    ctrl_pressed  = event->modifiers() & Qt::ControlModifier;
}
void SolidEditForm::mousePressEvent  (QMouseEvent *event){
    this->drag_base   = Pos(event->pos().x(),event->pos().y());
    this->first_click = Pos(event->pos().x(),event->pos().y());

    //操作
    if(this->state == MAKE_OBJECT::Edit){
        //移動
        if(this->GetHangedObject()->is<CPoint>() && this->controller->isSketcheing()){
            this->controller->hang_point = dynamic_cast<CPoint*>(this->GetHangedObject());
        }
    }else{
        //追加操作
        MakeObject();
    }
}
void SolidEditForm::mouseReleaseEvent(QMouseEvent *event){
    //保持点手放し
    if(state == MAKE_OBJECT::Edit){
        this->controller->hang_point = nullptr;
    }
    //ドラッグでなければ
    if(this->first_click == Pos(event->pos().x(),event->pos().y())){

        //オブジェクト選択
        if(state == MAKE_OBJECT::Edit){
            QVector<CObject*> objects;
            objects.push_back(this->GetHangedObject());
            objects.push_back(this->GetHangedFace());
            //シフト状態
            if(!shift_pressed)this->model->SelectedClear();//選択解除

            for(CObject* obj:objects){
                if(obj == nullptr)continue;
                //選択状態をトグル
                if(exist(this->model->GetSelected(),obj))this->model->RemoveSelected(obj);
                else this->model->AddSelected(obj);
                break;
            }
        }
        //右クリック
        if(event->button() == Qt::RightButton){
            menu.Show(event->globalPos());
        }
    }

    this->drag_base = Pos(0,0);
}
void SolidEditForm::mouseMoveEvent   (QMouseEvent *event){

    //スクリーン位置を取得
    this->screen_pos =  Pos(event->pos().x() - this->width()/2,-(event->pos().y() - this->height()/2)) * 2 * round;
    if(this->controller->isSketcheing()){
        //スケッチ中であれば平面上に点を配置
        Pos Line_base1 = this->screen_pos.Dot(this->controller->getCameraMatrix());
        Pos Line_base2 = Pos(0,0,1)      .Dot(this->controller->getCameraMatrix());
        this->mouse_pos  =  Collision::GetHitPosFaceToLine(this->controller->sketch_face->GetNorm(),
                                                           *this->controller->sketch_face->GetPointSequence(0),
                                                           Line_base1,Line_base2);
    }else{
        //カメラ角度から算出
        this->mouse_pos = this->screen_pos.Dot(this->controller->getCameraMatrix());
    }

    //シグナル発生
    CObject* hang = this->GetHangedObject();
    if(hang == nullptr){
        emit MousePosChanged(this->mouse_pos);
    }else if (!hang->is<CPoint>()){
        emit MousePosChanged(hang->GetNearPos(this->mouse_pos));
    }else{
        emit MousePosChanged(*dynamic_cast<CPoint*>(hang));
    }

    if(this->drag_base != Pos(0,0) && !this->controller->isSketcheing()){
        //カメラ角度変更
        this->controller->theta1 += static_cast<double>(event->pos().y() - this->drag_base.y())/SENSITIVITY;
        this->controller->theta2 += static_cast<double>(event->pos().x() - this->drag_base.x())/SENSITIVITY;
        this->controller->theta1 = Mod(this->controller->theta1,2*M_PI);
        this->controller->theta2 = Mod(this->controller->theta2,2*M_PI);
        this->drag_base = Pos(event->pos().x(),event->pos().y());
    }

    //最終保持座標を更新
    if(this->controller->hang_point != nullptr){
        this->controller->hang_point->MoveAbsolute(this->mouse_pos);
    }
    repaint();
}
void SolidEditForm::mouseDoubleClickEvent(QMouseEvent *event){
    //左ダブルクリック
    if(event->button() == Qt::LeftButton){
        CFace* f = this->GetHangedFace();
        if(f != nullptr){
            //スケッチ開始
            this->StartSketch(f);
        }else{
            this->mousePressEvent(event);
            this->mouseReleaseEvent(event);

        }
    }
}

void SolidEditForm::wheelEvent(QWheelEvent *event){
    this->round += static_cast<double>(event->angleDelta().y())/MOUSE_ZOOM_RATE;
    repaint();
}

void SolidEditForm::SetModel(CadModelCore* model){
    this->model = model;
    this->menu.SetModel(model);
    this->controller->SetModel(model);
    this->make_controller->SetModel(model);
    connect(this->model,SIGNAL(UpdateAnyAction()),this,SLOT(repaint()));
    //三平面作成
    this->controller->Create3Face();
    this->controller->CreateOrigin();
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
    if(this->controller->theta1 >  M_PI/2) this->controller->theta1 =  M_PI/2;
    if(this->controller->theta1 < -M_PI/2) this->controller->theta1 = -M_PI/2;
    if(this->round              <  0     ) this->round = 0.00001;
    this->camera = Pos(0,0,round).Dot(this->controller->getCameraMatrix());
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
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //オブジェクト描画
    glLineWidth(2);
    glColor3f(0,0,1);//青
    for(CBlock* block : this->model->GetBlocks())block->DrawGL(this->camera,this->center);
    for(CFace*  face  : this->model->GetFaces ())face ->DrawGL(this->camera,this->center);
    for(CEdge*  edge  : this->model->GetEdges ())edge ->DrawGL(this->camera,this->center);
    for(CPoint* pos   : this->model->GetPoints())pos  ->DrawGL(this->camera,this->center);

    //描画面と色のリスト作成
    QVector<std::pair<CFace*,QVector<int>>> faces;
    for(int i=0;i< 3;i++){
        faces.push_back(std::make_pair(CFace::base[i]    ,QVector<int>({0,0,0,0})));//右側面
    }
    faces.push_back(std::make_pair(this->controller->sketch_face,QVector<int>({1,1,0,1})));//スケッチ面
    //面の描画
    glLineWidth(2);
    for(std::pair<CFace*,QVector<int>>f: faces){
        if(f.first != nullptr){
            glColor4f(f.second[0], f.second[1], f.second[2],f.second[3]);
            f.first->DrawGL(this->camera,this->center);
        }
    }

    //選択オブジェクト描画
    glColor3f(0,1,1);//水色
    for(CObject* p: this->model->GetSelected()){
        p->DrawGL(this->camera,this->center);
    }

    //直下オブジェクトの描画
    glColor3f(1,1,1);//白
    CObject* hang_obj[] = {this->GetHangedObject(),this->GetHangedFace()} ;
    for(CObject* p: hang_obj){
        if(p != nullptr)p->DrawGL(this->camera,this->center);
    }

    //座標線の描画
    glLineWidth(4);
    glDepthFunc(GL_ALWAYS);//奥行き方向補正を無視
    for(int i=0;i<3;i++){
        glBegin(GL_LINES);
        glColor3f((i==0), (i==1), (i==2));

        Pos cc = (Pos(100-this->width(),100-this->height(),1)*round).Dot(this->controller->getCameraMatrix());
        Pos ex = cc + Pos(50*(i==0)*round, 50*(i==1)*round, 50*(i==2)*round);

        glVertex3f(cc.x(),cc.y(),cc.z());
        glVertex3f(ex.x(),ex.y(),ex.z());
        glEnd();
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
    this->camera.x() = round * std::cos(this->controller->theta2);
    this->camera.y() = round * std::sin(this->controller->theta1);
    this->camera.z() = round * std::sin(this->controller->theta2);
}

SolidEditForm::~SolidEditForm()
{
    delete ui;
    delete this->controller;
}
