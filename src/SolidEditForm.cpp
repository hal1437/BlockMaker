#include "SolidEditForm.h"
#include "ui_SolidEditForm.h"

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
    if(event->key() == Qt::Key_Up   )setCameraRotate(M_PI/2,0);
    if(event->key() == Qt::Key_Left )setCameraRotate(0,0);
    if(event->key() == Qt::Key_Right)setCameraRotate(0,-M_PI/2);
    if(event->key() == Qt::Key_Down )setCameraRotate(M_PI/4,-M_PI/4);
}

void SolidEditForm::mousePressEvent  (QMouseEvent *event){
    click_base = Pos(event->pos().x(),event->pos().y());
}
void SolidEditForm::mouseMoveEvent   (QMouseEvent *event){

    this->mouse_pos = Pos(event->pos().x() - this->width()/2,-(event->pos().y() - this->height()/2))*2;
    this->mouse_pos *= round;
    if(this->click_base != Pos(0,0)){
        //差分
        this->theta1 += static_cast<double>(event->pos().y() - click_base.y())/SENSITIVITY;
        this->theta2 += static_cast<double>(event->pos().x() - click_base.x())/SENSITIVITY;
        click_base = Pos(event->pos().x(),event->pos().y());
    }
    repaint();
}

void SolidEditForm::mouseReleaseEvent(QMouseEvent *event){
    click_base = Pos(0,0);
}
void SolidEditForm::wheelEvent(QWheelEvent *event){
    this->round += static_cast<double>(event->angleDelta().y())/MOUSE_ZOOM_RATE;
    repaint();
}

void SolidEditForm::SetModel(CadModelCore* model){
    this->model = model;
    this->controller->setModel(model);
    connect(this->model,SIGNAL(UpdateEdges (QVector<CEdge*>)) ,this,SLOT(CEdgeChanged(QVector<CEdge*>)));
    connect(this->model,SIGNAL(UpdateBlocks(QVector<CBlock*>)),this,SLOT(CBlockChanged(QVector<CBlock*>)));
}

void SolidEditForm::initializeGL(){
}

void SolidEditForm::resizeGL(int w, int h){
    glClearColor(0.7,0.7,0.7,1);

    glMatrixMode(GL_PROJECTION);  //行列モード切替
    glLoadIdentity();
    glOrtho(-w,
             w,
            -h,
             h,-100000000,100000000);

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
    glClearColor(0.7,0.7,0.7,1);//背景

    //平面、正面、右側面
    Face face[3] = {controller->getFrontFace(),controller->getTopFace(),controller->getSideFace()};
    glLineWidth(2);
    for(int i=0;i<3;i++){
        glBegin(GL_LINE_LOOP);
        glColor3f((i==0), (i==1), (i==2));
        for(int j=0;j<4;j++){
            glVertex3f(face[i].corner[j].x(),face[i].corner[j].y(), face[i].corner[j].z());
        }
        glEnd();
    }

    //中央線の描画
    for(int i=0;i<3;i++){
        glBegin(GL_LINES);
        glColor3f((i==0), (i==1), (i==2));
        glVertex3f(0,0,0);
        glVertex3f(50*(i==0), 50*(i==1), 50*(i==2));
        glEnd();
    }

    //直下面の選定
    Pos  hang_center = (Pos(0,0,1) + this->mouse_pos).Dot(Quat::getRotateXMatrix(theta1).Dot(Quat::getRotateYMatrix(theta2)));
    Face    hang_face = this->controller->getHangedFace  (hang_center,(this->camera - this->center).GetNormalize());
    CObject* hang_obj = this->controller->getHangedObject(hang_center,(this->camera - this->center).GetNormalize());

    //直下面の描画
    if(hang_obj == nullptr){
        glBegin(GL_LINE_LOOP);
        glColor3f(0,0,0);
        for(int j=0;j<4;j++){
            glVertex3f(hang_face.corner[j].x(),hang_face.corner[j].y(),hang_face.corner[j].z());
        }
        glEnd();
    }

    //エッジ描画
    for(int i=0;i<this->model->GetEdges().size();i++){
        CEdge* edge = this->model->GetEdges()[i];

        glBegin(GL_LINES);
        if(hang_obj == edge) glColor3f(1,0,0);
        else                 glColor3f(0,0,0);

        //線の分割描画
        for(double i=0;i<1;i += 1.0/CEdge::LINE_NEAR_DIVIDE){
            glVertex3f(edge->GetMiddleDivide(i).x(),
                       edge->GetMiddleDivide(i).y(),
                       edge->GetMiddleDivide(i).z());
        }
        glEnd();

        //端点の描画
        QVector<CPoint*> points = {edge->start,edge->end};
        for(int j=0;j<edge->GetMiddleCount();j++)points.push_back(edge->GetMiddle(j));
        for(int j=0;j<points.size();j++){
            glBegin(GL_LINE_LOOP);
            if(hang_obj == points[j])glColor3f(1,0,0);
            else                     glColor3f(0,0,0);

            //円の描画
            for(double k=0;k < 2*M_PI;k += M_PI/32){
                const int length = 5;
                Pos p = Pos(length*std::sin(k),length*std::cos(k),round).Dot(Quat::getRotateXMatrix(theta1).Dot(Quat::getRotateYMatrix(theta2)));
                glVertex3f((p + *points[j]).x(),(p + *points[j]).y(),(p + *points[j]).z());
            }
            glEnd();
        }

    }
    //ブロック描画
    for(int i=0;i<this->model->GetBlocks().size();i++){
        CBlock* block = this->model->GetBlocks()[i];

        for(int j=0;j<4;j++){
            glBegin(GL_LINE_LOOP);
            glColor3f(0,0,0);
            //glVertex3f(block->GetEdge(j)->start,block->GetEdge(j)->start,block->GetEdge(j)->start);
            glEnd();
        }
    }

    glFlush();
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
