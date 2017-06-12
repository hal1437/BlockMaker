#include "ObjectList.h"


void ObjectList::SetModel(CadModelCore* m){
    this->CadModelCoreInterface::model = m;
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdatePoints  ()),this,SLOT(UpdateObject()));
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateEdges   ()),this,SLOT(UpdateObject()));
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateFaces   ()),this,SLOT(UpdateObject()));
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateBlocks  ()),this,SLOT(UpdateObject()));
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(UpdateSelected()));
}

void ObjectList::UpdateObject  (){
    QVector<CPoint*> points = this->CadModelCoreInterface::model->GetPoints();
    QVector<CEdge *> edges  = this->CadModelCoreInterface::model->GetEdges();
    QVector<CFace *> faces  = this->CadModelCoreInterface::model->GetFaces();
    QVector<CBlock*> blocks = this->CadModelCoreInterface::model->GetBlocks();

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
    QTreeWidget(parent)
{
}

ObjectList::~ObjectList()
{
}
