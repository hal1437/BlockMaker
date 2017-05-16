#include "SolidView.h"


void SolidView::initializeGL(){
}

void SolidView::resizeGL(int w, int h){

    //glViewport(0, 0, w, h);  //ビューポートの設定

    glMatrixMode(GL_PROJECTION);  //行列モード切替
    glLoadIdentity();
    glOrtho(-w,w,-h,h,-100000000,100000000);
    //gluPerspective(120.0, (double)w / (double)h, 1.0, 100000.0); //視野の設定

    glMatrixMode(GL_MODELVIEW); //行列モードを戻す
    glLoadIdentity();
    gluLookAt(camera.x, camera.y, camera.z,
              center.x, center.y, center.z,
              0, 1, 0);
}

void SolidView::paintGL(){

    glClear(GL_COLOR_BUFFER_BIT);

    glLineWidth(2);
    for(CEdge* e:this->edges){
        glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(e->start->x, e->start->y, e->start->z);
        glVertex3f(e->end->x  , e->end->y  , e->end->z  );
        glEnd();
    }
    for(CBlock b:this->blocks){
        glBegin(GL_LINE_LOOP);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(b.GetClockworksPos(0).x, b.GetClockworksPos(0).y, 0);
        glVertex3f(b.GetClockworksPos(1).x, b.GetClockworksPos(1).y, 0);
        glVertex3f(b.GetClockworksPos(2).x, b.GetClockworksPos(2).y, 0);
        glVertex3f(b.GetClockworksPos(3).x, b.GetClockworksPos(3).y, 0);
        glEnd();
        glBegin(GL_LINE_LOOP);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(b.GetClockworksPos(0).x, b.GetClockworksPos(0).y, b.depth);
        glVertex3f(b.GetClockworksPos(1).x, b.GetClockworksPos(1).y, b.depth);
        glVertex3f(b.GetClockworksPos(2).x, b.GetClockworksPos(2).y, b.depth);
        glVertex3f(b.GetClockworksPos(3).x, b.GetClockworksPos(3).y, b.depth);
        glEnd();
        for(int i=0;i<4;i++){
            glBegin(GL_LINES);
            glColor3f(1.0, 0.0, 0.0);
            glVertex3f(b.GetClockworksPos(i).x, b.GetClockworksPos(i).y, b.GetClockworksPos(i).z);
            glVertex3f(b.GetClockworksPos(i).x, b.GetClockworksPos(i).y, b.GetClockworksPos(i).z + b.depth);
            glEnd();
        }

    }

    glLineWidth(5);

    //X
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0,0,0);
    glVertex3f(100,0,0);
    glEnd();
    //Y
    glBegin(GL_LINES);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0,0,0);
    glVertex3f(0,100,0);
    glEnd();
    //Z
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0,0,0);
    glVertex3f(0,0,100);
    glEnd();

    glFlush();
}

SolidView::SolidView(QWidget *parent) :
    QOpenGLWidget(parent)
{
    camera = Pos(100,100,100);
    center = Pos(0,0,0);
}

SolidView::~SolidView()
{
}

void SolidView::RefreshUI(QVector<CEdge*> edges,QVector<CBlock> blocks){
    this->edges  = edges;
    this->blocks = blocks;
    this->repaint();
}

