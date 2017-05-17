#include "SolidEditForm.h"
#include "ui_SolidEditForm.h"

void SolidEditForm::mousePressEvent  (QMouseEvent *event){
    click_base = Pos(event->pos().x(),event->pos().y());
}
void SolidEditForm::mouseMoveEvent   (QMouseEvent *event){
    //差分
    this->theta1 += static_cast<double>(event->pos().y() - click_base.y)/SENSITIVITY;
    this->theta2 += static_cast<double>(event->pos().x() - click_base.x)/SENSITIVITY;
    click_base = Pos(event->pos().x(),event->pos().y());

    //回転
    this->camera.x = round * std::cos(theta2);
    this->camera.y = round * std::sin(theta1);
    this->camera.z = round * std::sin(theta2);

    //qDebug() << "Thera1:" << this->theta1*(180/M_PI) << "Thera2:" << this->theta2*(180/M_PI);

    repaint();
}

void SolidEditForm::mouseReleaseEvent(QMouseEvent *event){
    click_base = Pos(0,0);
}
void SolidEditForm::wheelEvent(QWheelEvent *event){
    this->round += static_cast<double>(event->angleDelta().y())/MOUSE_ZOOM_RATE;
    this->camera.x = round * std::cos(theta2);
    this->camera.y = round * std::sin(theta1);
    this->camera.z = round * std::sin(theta2);
    repaint();
}

void SolidEditForm::setModel(CadModelCore* model){
    this->model = model;
}

void SolidEditForm::initializeGL(){
}

void SolidEditForm::resizeGL(int w, int h){
    glMatrixMode(GL_PROJECTION);  //行列モード切替
    glLoadIdentity();
    glOrtho(-w*round,w*round,-h*round,h*round,-100000000,100000000);

    glMatrixMode(GL_MODELVIEW); //行列モードを戻す
    glLoadIdentity();
    gluLookAt(camera.x, camera.y, camera.z,
              center.x, center.y, center.z,
              0, 1, 0);
}

void SolidEditForm::paintGL(){
    glMatrixMode(GL_PROJECTION);  //行列モード切替
    glLoadIdentity();
    glOrtho(-this->width()*round,this->width()*round,-this->height()*round,this->height()*round,-100000000,100000000);

    glMatrixMode(GL_MODELVIEW); //行列モードを戻す
    glLoadIdentity();
    gluLookAt(camera.x, camera.y, camera.z,
              center.x, center.y, center.z,
              0, 1, 0);

    glClear(GL_COLOR_BUFFER_BIT);


    for(int i=0;i<this->model->GetEdges().size();i++){
        CEdge* edge = this->model->GetEdges()[i];

        if(edge->is<CLine>()){
            glBegin(GL_LINE_LOOP);
            glColor3f(1,0,0);
            glVertex3f(edge->start->x,edge->start->y,edge->start->z);
            glVertex3f(edge->end->x,edge->end->y,edge->end->z);
            glEnd();
        }
    }

    //平面、正面、右側面
    glLineWidth(2);
    const int RANGE = 100;
    for(int i=0;i<3;i++){
        int PP[12] = {    0,     0,     0,     0,
                      RANGE,-RANGE,-RANGE, RANGE,
                      RANGE, RANGE,-RANGE,-RANGE};
        glBegin(GL_LINE_LOOP);
        glColor3f((i==0), (i==1), (i==2));
        for(int j=0;j<4;j++){
            glVertex3f( PP[(i*4+j)%12],PP[(i*4+j+4)%12], PP[(i*4+j+8)%12]);
        }
        glEnd();
    }
    glFlush();
}


SolidEditForm::SolidEditForm(QWidget *parent) :
    QOpenGLWidget(parent),
    ui(new Ui::SolidEditForm)
{
    ui->setupUi(this);

    camera = Pos(round,0,0);
    center = Pos(0,0,0);
    this->camera.x = round * std::cos(theta2);
    this->camera.y = round * std::sin(theta1);
    this->camera.z = round * std::sin(theta2);
}

SolidEditForm::~SolidEditForm()
{
    delete ui;
}
