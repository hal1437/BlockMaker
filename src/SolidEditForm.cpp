#include "SolidEditForm.h"
#include "ui_SolidEditForm.h"

void SolidEditForm::MakeObject(){
    CObject* hanged = this->GetHangedObject();
    if(state != MAKE_OBJECT::Edit ){
        //生成
        this->make_controller->Making(state,this->mouse_pos,hanged);
        //最終点を保持
        CPoint::hanged = this->make_controller->GetLastPos();

        this->controller->Create3Face();

    }
    this->repaint();
}

CFace* SolidEditForm::GetHangedFace(){
    if(this->controller->isSketcheing())return nullptr;
    Pos  hang_center = (Pos(0,0,1) + this->screen_pos).Dot(this->controller->getCameraMatrix());
    return  this->controller->getHangedFace(this->center + hang_center,
                                            this->camera + hang_center,
                                            this->round);
}
CObject* SolidEditForm::GetHangedObject(){
    Pos  hang_center = this->center + (Pos(0,0,1) + this->screen_pos).Dot(this->controller->getCameraMatrix());
    return this->controller->getHangedObject(hang_center,(this->camera - this->center).GetNormalize(),this->round);
}
void SolidEditForm::ColorSelect(CObject* obj){
    CObject* hang = this->GetHangedObject();
    if     (hang == obj                          )glColor3f(1,0,0);
    else if(exist(this->model->GetSelected(),obj))glColor3f(0,1,1);
    else                                          glColor3f(0,0,1);
}


void SolidEditForm::keyPressEvent    (QKeyEvent *event){
    //キーイベント
    if(!this->controller->isSketcheing()){
        if(event->key() == Qt::Key_Up    )this->SetCameraRotate(M_PI/2,0);  //平面
        if(event->key() == Qt::Key_Left  )this->SetCameraRotate(0,0);       //正面
        if(event->key() == Qt::Key_Right )this->SetCameraRotate(0,-M_PI/2); //右側面
        if(event->key() == Qt::Key_Down  ){        //視点リセット
            this->SetCameraRotate(M_PI/4,-M_PI/4);
            this->SetCameraCenter(Pos(0,0,0));
            this->SetZoomRate(1.0);
        }
    }
    //キー状態保存
    shift_pressed = event->modifiers() & Qt::ShiftModifier;
    ctrl_pressed  = event->modifiers() & Qt::MetaModifier;

    //スケッチ終了
    if(event->key() == Qt::Key_Escape){
        this->make_controller->Escape();
        CPoint::hanged      = nullptr;
        this->controller->projection_face = nullptr;
    }

    this->repaint();
}
void SolidEditForm::StartProjection(CFace* face){
    if(this->controller->isSketcheing())return;
    this->controller->projection_face = face;
    this->model->SelectedClear();//選択解除

    //法線ベクトルを近い方に変更
    Pos cross = face->GetNorm().GetNormalize();
    if(cross.DotPos(this->camera - this->center) <  0){
        cross = -cross;
    }
    //カメラ変換
    double theta1_ = std::atan2(cross.y(),std::sqrt(cross.x()*cross.x()+cross.z()*cross.z()));
    double theta2_ = std::atan2(-cross.x(),cross.z());
    this->SetCameraRotate(theta1_,theta2_);
}
void SolidEditForm::SetCameraRotate(double theta1,double theta2){
    TimeDivider *p1,*p2;
    p1 = TimeDivider::TimeDivide(this->controller->theta1  ,theta1,500);
    p2 = TimeDivider::TimeDivide(this->controller->theta2  ,theta2,500);
    connect(p1,SIGNAL(PerTime()),this,SLOT(repaint()));
    connect(p2,SIGNAL(PerTime()),this,SLOT(repaint()));
}

void SolidEditForm::SetCameraCenter(Pos point){
    TimeDivider* t[3];
    for(int i = 0;i< 3;i++){
        t[i] = TimeDivider::TimeDivide(this->center.mat[i],point.mat[i],500);
        connect(t[i],SIGNAL(PerTime()),this,SLOT(repaint()));
    }
}
void SolidEditForm::SetZoomRate(double round){
    TimeDivider* r = TimeDivider::TimeDivide(this->round,1.0,500);
    connect(r,SIGNAL(PerTime()),this,SLOT(repaint()));
}
void SolidEditForm::keyReleaseEvent  (QKeyEvent *event){
    shift_pressed = event->modifiers() & Qt::ShiftModifier;
    ctrl_pressed  = event->modifiers() & Qt::MetaModifier;
}
void SolidEditForm::mousePressEvent  (QMouseEvent *event){
    this->drag_base   = Pos(event->pos().x(),event->pos().y());
    this->first_click = Pos(event->pos().x(),event->pos().y());

    //操作
    if(this->state == MAKE_OBJECT::Edit || this->ctrl_pressed){
        //移動
        if(this->GetHangedObject()->is<CPoint>() && this->controller->isSketcheing()){
            CPoint::hanged = dynamic_cast<CPoint*>(this->GetHangedObject());
        }
    }else{
        //追加操作
        MakeObject();
    }
}
void SolidEditForm::mouseReleaseEvent(QMouseEvent *event){
    //保持点手放し
    if(state == MAKE_OBJECT::Edit){
        CPoint::hanged = nullptr;
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
        this->mouse_pos  =  Collision::GetHitPosFaceToLine(this->controller->projection_face->GetNorm(),
                                                           *this->controller->projection_face->GetAllChildren()[0],
                                                           Line_base1,Line_base2) + this->center;
    }else{
        //カメラ角度から算出
        this->mouse_pos = this->screen_pos.Dot(this->controller->getCameraMatrix()) + this->center;
    }

    Pos delta = this->screen_pos.Dot(this->controller->getCameraMatrix());

    //シグナル発生
    CObject* hang = this->GetHangedObject();
    if(hang != nullptr){

        this->mouse_pos = hang->GetNearLine(this->camera + delta,this->center + delta);
    }
    emit MousePosChanged(this->mouse_pos);


    //ドラッグであれば
    if(this->drag_base != Pos(0,0) ){
        if(ctrl_pressed){
            Pos delta(-(event->pos().x()-this->drag_base.x()) ,event->pos().y()-this->drag_base.y());
            this->center = (delta*round).Dot(this->controller->getCameraMatrix()) + this->center;
        }else if(CPoint::hanged == nullptr){
            //カメラ角度変更
            this->controller->theta1 += static_cast<double>(event->pos().y() - this->drag_base.y())/SENSITIVITY;
            this->controller->theta2 += static_cast<double>(event->pos().x() - this->drag_base.x())/SENSITIVITY;
            this->controller->theta1 = Mod(this->controller->theta1,2*M_PI);
            this->controller->theta2 = Mod(this->controller->theta2,2*M_PI);
        }
        this->drag_base = Pos(event->pos().x(),event->pos().y());
    }

    //保持点の座標を更新
    if(CPoint::hanged != nullptr){
        CPoint::hanged->MoveAbsolute(this->mouse_pos);
    }
    repaint();
}
void SolidEditForm::mouseDoubleClickEvent(QMouseEvent *event){
    //左ダブルクリック
    if(event->button() == Qt::LeftButton){
        CFace* f = this->GetHangedFace();
        if(f != nullptr){
            //スケッチ開始
            this->StartProjection(f);
        }else{
            this->mousePressEvent(event);
            this->mouseReleaseEvent(event);

        }
    }

    //注視点移動
    if(state == MAKE_OBJECT::Edit &&
       !this->model->GetSelected().isEmpty() && this->model->GetSelected().first()->is<CPoint>()){
        this->SetCameraCenter(*dynamic_cast<CPoint*>(this->model->GetSelected().first()));
    }
}

void SolidEditForm::wheelEvent(QWheelEvent *event){
    const double zoom_rate = 0.999;
    int count = std::abs(event->angleDelta().y());
    for(int i=0;i<count;i++){
        if(event->angleDelta().y() > 0)this->round /= zoom_rate;
        else                           this->round *= zoom_rate;
    }
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
    //深度テスト有効化
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void SolidEditForm::resizeGL(int w, int h){
}

void SolidEditForm::paintGL(){

    //カメラ調整
    if(this->controller->theta1 >  M_PI/2 - CAMERA_ANGLE_LIMIT) this->controller->theta1 =  M_PI/2 - CAMERA_ANGLE_LIMIT;
    if(this->controller->theta1 < -M_PI/2 + CAMERA_ANGLE_LIMIT) this->controller->theta1 = -M_PI/2 + CAMERA_ANGLE_LIMIT;
    if(this->round              <  0     ) this->round = 0.00001;
    this->camera = this->center + Pos(0,0,round).Dot(this->controller->getCameraMatrix());

    //OpenGLカメラ設定
    glMatrixMode(GL_PROJECTION);  //行列モード切替
    glLoadIdentity();
    glOrtho(-this->width() *(round),
             this->width() *(round),
            -this->height()*(round),
             this->height()*(round),-1.0e5,1.0e5);
    glMatrixMode(GL_MODELVIEW); //行列モードを戻す
    glLoadIdentity();
    gluLookAt(camera.x(), camera.y(), camera.z(),
              center.x(), center.y(), center.z(),
                       0,          1,          0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.7,0.7,0.7,1);//背景

    //カメラ変換行列
    Quat quat = Quat::getRotateXMatrix(this->controller->theta1).Dot(Quat::getRotateYMatrix(this->controller->theta2));

    //直下オブジェクト選定
    CObject* hanged = this->GetHangedFace();
    if(hanged == nullptr)hanged = this->GetHangedObject();

    //幾何拘束描画
    QVector<std::pair<Pos,QVector<QString>>> rest_maps;
    for(Restraint* rest : this->model->GetRestraints()){
        for(Pos pp:rest->GetIconPoint()){
            bool exist = false;
            for(std::pair<Pos,QVector<QString>>& p: rest_maps){
                if(p.first == pp){
                    p.second.push_back(rest->GetIconPath());
                    exist = true;
                    break;
                }
            }
            if(exist != true){
                rest_maps.push_back(std::make_pair(pp,QVector<QString>{rest->GetIconPath()}));
            }
        }
    }
    for(std::pair<Pos,QVector<QString>> ss : rest_maps){
        for(int i=0;i<ss.second.size();i++){
            QImage img(ss.second[i]);
            QImage glimg = QGLWidget::convertToGLFormat(img);
            Pos cp = ss.first + Pos(30*i,0,0).Dot(quat);
            glRasterPos3f(cp.x(),cp.y(),cp.z());
            glDrawPixels(img.width(), img.height(), GL_RGBA, GL_UNSIGNED_BYTE, glimg.bits());
        }
    }
    const int ALL_OBJECT_WIDTH = 3;
    glLineWidth(ALL_OBJECT_WIDTH*2/3);

    glColor3f(0,0,1);
    glDepthFunc(GL_LEQUAL);
    //オブジェクト描画：線
    for(CEdge*  edge  : this->model->GetEdges ())paintObject(edge  ,{0,0,1,1},ALL_OBJECT_WIDTH); //通常の物体
    if(hanged->is<CEdge>())                      paintObject(hanged,{1,1,1,1},ALL_OBJECT_WIDTH); //選択物体(線)
    //オブジェクト描画：点
    for(CPoint* pos   : this->model->GetPoints())paintObject(pos   ,{0,0,1,1},ALL_OBJECT_WIDTH); //通常の物体
    if(hanged->is<CPoint>())                     paintObject(hanged,{1,1,1,1},ALL_OBJECT_WIDTH); //選択物体(線)
    //オブジェクト描画：平面
    for(CFace*  face  : this->model->GetFaces ())face->DrawMeshGL(); //非透過の面
    for(CFace*  face  : this->model->GetFaces ())if(!face->isFaceBlend() && !exist(CFace::base,face))paintObject(face  ,{0,0,1,1},ALL_OBJECT_WIDTH); //非透過の面
    for(CFace*  base  : CFace::base             )paintObject(base  ,{std::abs(base->GetNorm().x()),std::abs(base->GetNorm().y()),std::abs(base->GetNorm().z()),1},ALL_OBJECT_WIDTH); //三平面
    for(CFace*  face  : this->model->GetFaces ())if( face->isFaceBlend() && !exist(CFace::base,face))paintObject(face  ,{0,0,1,1},ALL_OBJECT_WIDTH); //透過の面
    //直下面
    if(hanged->is<CFace>())                      paintObject(hanged,{1,1,1,1},ALL_OBJECT_WIDTH);//選択物体(平面)
    //投影面
    if(this->controller->isSketcheing())         paintObject(this->controller->projection_face,{1,1,0,1},ALL_OBJECT_WIDTH);//選択物体(平面)
    //オブジェクト描画：STL
    for(CStl*   stl   : this->model->GetStls  ())paintObject(stl  ,{0.5,0.5,0.5,1},ALL_OBJECT_WIDTH);//STL

    //移動予測
    if(this->move_diag != nullptr && this->move_diag->isVisible()){
        this->move_diag->DrawTranslated(camera,center);
    }


    //座標線の描画
    glLineWidth(3);
    glDepthFunc(GL_ALWAYS);//奥行き方向補正を無視
    Pos cc = ((Pos(100,100,1)-Pos(this->width(),this->height(),0))*round).Dot(quat) + this->center;
    for(int i=0;i<3;i++){
        glBegin(GL_LINES);
        glColor3f((i==0), (i==1), (i==2));

        Pos ex = cc + Pos(50*(i==0)*round, 50*(i==1)*round, 50*(i==2)*round);

        glVertex3f(cc.x(),cc.y(),cc.z());
        glVertex3f(ex.x(),ex.y(),ex.z());
        glEnd();
    }
    //投影法線の描画
    if(this->controller->isSketcheing()){
        glBegin(GL_LINES);
        glColor3f(1,1,0);
        glVertex3f(cc.x(),cc.y(),cc.z());
        glVertex3f(cc.x() + 50 * this->controller->projection_face->GetNorm().x() * round,
                   cc.y() + 50 * this->controller->projection_face->GetNorm().y() * round,
                   cc.z() + 50 * this->controller->projection_face->GetNorm().z() * round);
        glEnd();
    }

    glFlush();
}
void SolidEditForm::paintObject(CObject* obj,QVector<double> color,int tick){
    //色を保存
    int old_width;
    float old_color[4];

    //選択オブジェクトであれば
    if(exist(this->model->GetSelected(),obj)){
        color[0] = 0;
        color[1] = 1;
        color[2] = 1;
        color[3] = 1;
    }

    //色と線の太さを保存
    glGetIntegerv(GL_LINE_WIDTH   ,&old_width);
    glGetFloatv  (GL_CURRENT_COLOR,old_color);

    //色と線の太さを設定
    glLineWidth(tick);
    glColor4f(color[0],color[1],color[2],color[3]);//引数の色に設定

    //描画
    obj->DrawGL(this->camera,this->center); // 選択面の描画

    //色と線の太さを復元
    glLineWidth(old_width);
    glColor4f(old_color[0],old_color[1],old_color[2], old_color[3]);
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
