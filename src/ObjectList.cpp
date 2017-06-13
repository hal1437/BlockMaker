#include "ObjectList.h"

void ObjectList::AddBlockToTree(CBlock* block,QTreeWidgetItem* parent,int index){
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,QString("Face:") + QString::number(index));
    item->setIcon(0,QIcon(":/ToolImages/Rect.png"));

    for(int i = 0;i<block->faces.size();i++){
        AddFaceToTree(block->faces[i],parent,index+1);
    }
    item->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),block));
    if(parent == nullptr)this->addTopLevelItem(item);
    else                 parent->addChild(item);
}
void ObjectList::AddFaceToTree(CFace*  face ,QTreeWidgetItem* parent,int index){
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,QString("Face:") + QString::number(index));
    item->setIcon(0,QIcon(":/ToolImages/Rect.png"));

    for(int i = 0;i<face->edges.size();i++){
        AddEdgeToTree(face->edges[i],item,index+1);
    }
    item->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),face));
    if(parent == nullptr)this->addTopLevelItem(item);
    else                 parent->addChild(item);
}
void ObjectList::AddEdgeToTree(CEdge* edge,QTreeWidgetItem* parent,int index){
    QTreeWidgetItem* item = new QTreeWidgetItem();
    std::pair<std::string,std::string> p;
    if(edge->is<CLine>  ())p = std::make_pair("Line"  ,":/ToolImages/Line.png");
    if(edge->is<CArc>   ())p = std::make_pair("Arc"   ,":/ToolImages/Arc.png");
    if(edge->is<CSpline>())p = std::make_pair("Spline",":/ToolImages/Spline.png");
    item->setText(0,QString(p.first.c_str()) + ":" + QString::number(index));
    item->setIcon(0,QIcon(p.second.c_str()));

    for(int i=0;i<edge->GetPosSequenceCount();i++){
        AddPointToTree(edge->GetPosSequence(i),item,index+1);
    }
    if(exist(this->CadModelCoreInterface::model->GetSelected(),edge)){
        item->setSelected(true);
    }else{
        item->setSelected(false);
    }
    if(parent == nullptr)this->addTopLevelItem(item);
    else                 parent->addChild(item);
}
void ObjectList::AddPointToTree(CPoint* point,QTreeWidgetItem* parent,int index){
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,QString("Point:") + QString::number(index));
    item->setIcon(0,QIcon(":/ToolImages/Dot.png"));
    item->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),point));
    if(parent == nullptr)this->addTopLevelItem(item);
    else                 parent->addChild(item);
}
void ObjectList::pullSelectedBlock(CBlock* block,QTreeWidgetItem* current){
    if(current->isSelected())this->CadModelCoreInterface::model->AddSelected(block);
    else                     this->CadModelCoreInterface::model->RemoveSelected(block);
    for(int i=0;i<current->childCount();i++){
        pullSelectedFace(block->faces[i],current->child(i));
    }
}
void ObjectList::pullSelectedFace (CFace*  face ,QTreeWidgetItem* current){
    if(current->isSelected())this->CadModelCoreInterface::model->AddSelected(face);
    else                     this->CadModelCoreInterface::model->RemoveSelected(face);
    for(int i=0;i<current->childCount();i++){
        pullSelectedEdge(face->edges[i],current->child(i));
    }
}
void ObjectList::pullSelectedEdge (CEdge*  edge ,QTreeWidgetItem* current){
    if(current->isSelected()){
        this->CadModelCoreInterface::model->AddSelected(edge);
    }else{
        this->CadModelCoreInterface::model->RemoveSelected(edge);
    }
    for(int i=0;i<current->childCount();i++){
        pullSelectedPoint(edge->GetPosSequence(i),current->child(i));
    }
}
void ObjectList::pullSelectedPoint(CPoint* point,QTreeWidgetItem* current){
    if(current->isSelected())this->CadModelCoreInterface::model->AddSelected(point);
    else                     this->CadModelCoreInterface::model->RemoveSelected(point);
}

void ObjectList::SetModel(CadModelCore* m){
    this->CadModelCoreInterface::model = m;
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdatePoints  ()),this,SLOT(UpdateObject()));
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateEdges   ()),this,SLOT(UpdateObject()));
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateFaces   ()),this,SLOT(UpdateObject()));
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateBlocks  ()),this,SLOT(UpdateObject()));
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));
}

void ObjectList::UpdateObject  (){
    this->points = this->CadModelCoreInterface::model->GetPoints();
    this->edges  = this->CadModelCoreInterface::model->GetEdges();
    this->faces  = this->CadModelCoreInterface::model->GetFaces();
    this->blocks = this->CadModelCoreInterface::model->GetBlocks();

    //排他処理
    for(CEdge* edge: this->edges){
        for(int i =0;i<edge->GetPosSequenceCount();i++)this->points.removeAll(edge->GetPosSequence(i));
    }
    for(CFace* face: this->faces){
        for(CEdge* e : face->edges)this->edges.removeAll(e);
    }
    for(CBlock* block: this->blocks){
        for(CFace* f : block->faces)this->faces.removeAll(f);
    }

    //ツリーにセット
    this->clear();
    for(int i=0;i<this->blocks.size();i++)AddBlockToTree(this->blocks[i],nullptr,i);
    for(int i=0;i<this->faces .size();i++)AddFaceToTree (this->faces [i],nullptr,i);
    for(int i=0;i<this->edges .size();i++)AddEdgeToTree (this->edges [i],nullptr,i);
    for(int i=0;i<this->points.size();i++)AddPointToTree(this->points[i],nullptr,i);
}

void ObjectList::PullSelected(){
    //this <- model
    //ツリーにセット
    disconnect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//一時的にコネクト解除
    disconnect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
    this->clear();
    for(int i=0;i<this->blocks.size();i++)AddBlockToTree(this->blocks[i],nullptr,i);
    for(int i=0;i<this->faces .size();i++)AddFaceToTree (this->faces [i],nullptr,i);
    for(int i=0;i<this->edges .size();i++)AddEdgeToTree (this->edges [i],nullptr,i);
    for(int i=0;i<this->points.size();i++)AddPointToTree(this->points[i],nullptr,i);
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//再コネクト
    connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
}

void ObjectList::PushSelected(){
    //this -> model
    int count = 0;

    disconnect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//一時的にコネクト解除
    disconnect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
    for(int i=0;i<this->blocks.size();i++,count++)pullSelectedBlock(this->blocks[i],this->topLevelItem(count));
    for(int i=0;i<this->faces .size();i++,count++)pullSelectedFace (this->faces [i],this->topLevelItem(count));
    for(int i=0;i<this->edges .size();i++,count++)pullSelectedEdge (this->edges [i],this->topLevelItem(count));
    for(int i=0;i<this->points.size();i++,count++)pullSelectedPoint(this->points[i],this->topLevelItem(count));
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//再コネクト
    connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
}

ObjectList::ObjectList(QWidget *parent) :
    QTreeWidget(parent)
{
    connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
}

ObjectList::~ObjectList()
{
}
