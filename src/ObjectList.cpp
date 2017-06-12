#include "ObjectList.h"


void ObjectList::SetModel(CadModelCore* m){
    this->model = m;
    connect(this->model,SIGNAL(UpdatePoints  ()),this,SLOT(UpdateObject()));
    connect(this->model,SIGNAL(UpdateEdges   ()),this,SLOT(UpdateObject()));
    connect(this->model,SIGNAL(UpdateFaces   ()),this,SLOT(UpdateObject()));
    connect(this->model,SIGNAL(UpdateBlocks  ()),this,SLOT(UpdateObject()));
    connect(this->model,SIGNAL(UpdateSelected()),this,SLOT(UpdateSelected()));
}

void ObjectList::UpdateObject  (){
    QVector<CPoint*> points = this->model->GetPoints();
    QVector<CEdge *> edges  = this->model->GetEdges();
    QVector<CFace *> faces  = this->model->GetFaces();
    QVector<CBlock*> blocks = this->model->GetBlocks();

    //排他処理
    for(CEdge* edge: edges){
        for(int i =0;i<edge->GetPosSequenceCount();i++)points.removeAll(edge->GetPosSequence(i));
    }
//    for(CFace* face: faces){
//        for(int i =0;i<face->corner;i++)points.removeAll(edge->GetPosSequence(i));
//    }

}

void ObjectList::UpdateSelected(){

}

ObjectList::ObjectList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ObjectList)
{
    ui->setupUi(this);
}

ObjectList::~ObjectList()
{
    delete ui;
}
