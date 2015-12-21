#include "CadEditForm.h"
#include "ui_CadEditForm.h"

void CadEditForm::AddObject(CObject* obj){
    objects.push_back(obj);
}
void CadEditForm::RemoveObject(CObject* obj){
    auto it = std::find(objects.begin(),objects.end(),obj);
    if(it != objects.end()){
        delete *it;
        objects.erase(it);
    }
    repaint();
}

void CadEditForm::paintEvent(QPaintEvent * event){
    QPainter paint(this);
    paint.fillRect(0,0,this->width(),this->height(),Qt::white);
    for(int i=0;i<this->objects.size();i++){
        objects[i]->Draw(paint);
    }
}

void CadEditForm::mouseMoveEvent   (QMouseEvent* event){
    CObject::mouse_over = Pos(event->pos().x(),event->pos().y());
    CObject* answer = Selecting();
    repaint();
    emit MovedMouse(event,answer);
}

CadEditForm::CadEditForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CadEditForm)
{
    ui->setupUi(this);
    setMouseTracking(true);
}

CadEditForm::~CadEditForm()
{
    delete ui;
}

CObject* CadEditForm::Selecting(){
    for(CObject* obj:objects){
        if(obj->Selecting())return obj;
    }
    return nullptr;
}
