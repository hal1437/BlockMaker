#include "ObjectList.h"

QIcon ObjectList::getIcon(CObject *obj){
    QString filepath;
    if(obj->is<CBlock>()){
        if(dynamic_cast<CBlock*>(obj)->isVisibleMesh()){
            filepath = ":/ToolImages/BlocksMesh";
        }else{
            filepath = ":/ToolImages/Blocks";
        }
    }
    if(obj->is<CFace>()){
        filepath = ":/ToolImages/Face";
    }
    if(obj->is<CEdge>()){
        if(obj->is<CLine>  ())filepath = ":/ToolImages/Line";
        if(obj->is<CArc>   ())filepath = ":/ToolImages/Arc";
        if(obj->is<CSpline>())filepath = ":/ToolImages/Spline";
    }
    if(obj->is<CPoint>()){
        filepath = ":/ToolImages/Dot";
    }

    //不可視は薄くする
    if(obj->isVisible())filepath += ".png";
    else                filepath += "_disabled.png";

    return QIcon(filepath);
}

void ObjectList::mouseReleaseEvent(QMouseEvent* event){

    if(event->button() == Qt::RightButton){
        QVector<CObject*> selected = this->CadModelCoreInterface::model->GetSelected();
        this->menu = new QMenu(this);
        this->delete_action          = this->menu->addAction("削除");
        connect(this->delete_action,SIGNAL(triggered(bool)),this,SLOT(Delete(bool)));

        QVector<CEdge*> edges = this->CadModelCoreInterface::model->GetEdges();
        if(std::any_of(selected.begin(),selected.end(),[&](CObject* obj){return obj->is<CArc>();})){
            this->reverse_action         = this->menu->addAction("円弧反転");
            connect(this->reverse_action   ,SIGNAL(triggered(bool)),this,SLOT(ReverseArc(bool)));
        }

        for(CObject* obj:selected){
            if(!obj->isVisible()){
                this->visible_action         = this->menu->addAction("表示");
                connect(this->visible_action        ,SIGNAL(triggered(bool)),this,SLOT(SetVisible(bool)));
                break;
            }
        }
        for(CObject* obj:selected){
            if(obj->isVisible()){
                this->invisible_action       = this->menu->addAction("非表示");
                connect(this->invisible_action      ,SIGNAL(triggered(bool)),this,SLOT(SetInvisible(bool)));
                break;
            }
        }
        for(CObject* obj:selected){
            if(obj->is<CBlock>() && !dynamic_cast<CBlock*>(obj)->isVisibleMesh()){
                this->visible_mesh_action   = this->menu->addAction("メッシュ表示");
                connect(this->visible_mesh_action  ,SIGNAL(triggered(bool)),this,SLOT(SetVisibleMesh(bool)));
                break;
            }
        }
        for(CObject* obj:selected){
            if(obj->is<CBlock>() && dynamic_cast<CBlock*>(obj)->isVisibleMesh()){
                this->invisible_mesh_action = this->menu->addAction("メッシュ非表示");
                connect(this->invisible_mesh_action,SIGNAL(triggered(bool)),this,SLOT(SetInvisibleMesh(bool)));
                break;
            }
        }
        this->setSelectionMode(QTreeWidget::ExtendedSelection);
        this->menu->exec(event->globalPos());
    }
}

void ObjectList::AddBlockToTree(CBlock* block,QTreeWidgetItem* parent,int index){
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,QString("Block:") + QString::number(index));
    QIcon icon;

    item->setIcon(0,getIcon(block));

    for(int i = 0;i<block->faces.size();i++){
        AddFaceToTree(block->faces[i],item,i+1);
    }
    item->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),block));

    if(parent == nullptr)this->addTopLevelItem(item);
    else                 parent->addChild(item);
}
void ObjectList::AddFaceToTree(CFace*  face ,QTreeWidgetItem* parent,int index){
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0,QString("Face:") + QString::number(index));
    item->setIcon(0,getIcon(face));

    for(int i = 0;i<face->edges.size();i++){
        AddEdgeToTree(face->edges[i],item,i+1);
    }
    item->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),face));
    if(parent == nullptr)this->addTopLevelItem(item);
    else                 parent->addChild(item);
}
void ObjectList::AddEdgeToTree(CEdge* edge,QTreeWidgetItem* parent,int index){
    QTreeWidgetItem* item = new QTreeWidgetItem();
    std::string s;
    if(edge->is<CLine>  ())s = "Line";
    if(edge->is<CArc>   ())s = "Arc";
    if(edge->is<CSpline>())s = "Spline";
    item->setText(0,QString(s.c_str()) + ":" + QString::number(index));
    item->setIcon(0,getIcon(edge));

    for(int i=0;i<edge->GetChildCount();i++){
        AddPointToTree(edge->GetPoint(i),item,i+1);
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
    if(point == nullptr){
        item->setText(0,QString("NullPointer:") + QString::number(index));
    }else{
        item->setText(0,QString("Point:") + QString::number(index));
        item->setIcon(0,getIcon(point));
        item->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),point));
    }
    if(parent == nullptr)this->addTopLevelItem(item);
    else                 parent->addChild(item);
}
void ObjectList::pushSelectedBlock(CBlock* block,QTreeWidgetItem* current){
    current->setIcon(0,this->getIcon(block));
    if(current->isSelected())this->CadModelCoreInterface::model->AddSelected(block);
    else                     this->CadModelCoreInterface::model->RemoveSelected(block);
    for(int i=0;i<current->childCount();i++){
        pushSelectedFace(block->faces[i],current->child(i));
    }
}
void ObjectList::pushSelectedFace (CFace*  face ,QTreeWidgetItem* current){
    current->setIcon(0,this->getIcon(face));
    if(current->isSelected())this->CadModelCoreInterface::model->AddSelected(face);
    else                     this->CadModelCoreInterface::model->RemoveSelected(face);
    for(int i=0;i<current->childCount();i++){
        pushSelectedEdge(face->edges[i],current->child(i));
    }
}
void ObjectList::pushSelectedEdge (CEdge*  edge ,QTreeWidgetItem* current){
    current->setIcon(0,this->getIcon(edge));
    if(current->isSelected()){
        this->CadModelCoreInterface::model->AddSelected(edge);
    }else{
        this->CadModelCoreInterface::model->RemoveSelected(edge);
    }
    for(int i=0;i<current->childCount();i++){
        pushSelectedPoint(edge->GetPoint(i),current->child(i));
    }
}
void ObjectList::pushSelectedPoint(CPoint* point,QTreeWidgetItem* current){
    current->setIcon(0,this->getIcon(point));
    if(current->isSelected()){
        this->CadModelCoreInterface::model->AddSelected(point);
    }
}
void ObjectList::pullSelectedBlock(CBlock* block,QTreeWidgetItem* current){
    current->setIcon(0,this->getIcon(block));
    current->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),block));
    for(int i=0;i<current->childCount();i++){
        pullSelectedFace(block->faces[i],current->child(i));
    }
}
void ObjectList::pullSelectedFace (CFace*  face ,QTreeWidgetItem* current){
    current->setIcon(0,this->getIcon(face));
    current->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),face));
    for(int i=0;i<current->childCount();i++){
        pullSelectedEdge(face->edges[i],current->child(i));
    }
}
void ObjectList::pullSelectedEdge (CEdge*  edge ,QTreeWidgetItem* current){
    current->setIcon(0,this->getIcon(edge));
    current->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),edge));
    for(int i=0;i<current->childCount();i++){
        pullSelectedPoint(edge->GetPoint(i),current->child(i));
    }
}
void ObjectList::pullSelectedPoint(CPoint* point,QTreeWidgetItem* current){
    current->setIcon(0,this->getIcon(point));
    current->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),point));
}


void ObjectList::SetModel(CadModelCore* m){
    this->CadModelCoreInterface::model = m;
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateAnyObject()),this,SLOT(UpdateObject()));
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected ()),this,SLOT(PullSelected()));
}

void ObjectList::UpdateObject  (){
    this->points = this->CadModelCoreInterface::model->GetPoints();
    this->edges  = this->CadModelCoreInterface::model->GetEdges();
    this->faces  = this->CadModelCoreInterface::model->GetFaces();
    this->blocks = this->CadModelCoreInterface::model->GetBlocks();

    //排他処理
    for(CEdge* edge: this->edges){
        for(int i =0;i<edge->GetChildCount();i++)this->points.removeAll(edge->GetPoint(i));
    }
    for(CFace* face: this->faces){
        for(CEdge* e : face->edges)this->edges.removeAll(e);
    }
    for(CBlock* block: this->blocks){
        for(CFace* f : block->faces)this->faces.removeAll(f);
    }

    //ツリーにセット
    disconnect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//一時的にコネクト解除
    disconnect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
    this->clear();
    for(int i=0;i<this->blocks.size();i++)AddBlockToTree(this->blocks[i],nullptr,i+1);
    for(int i=0;i<this->faces .size();i++)AddFaceToTree (this->faces [i],nullptr,i+1);
    for(int i=0;i<this->edges .size();i++)AddEdgeToTree (this->edges [i],nullptr,i+1);
    for(int i=0;i<this->points.size();i++)AddPointToTree(this->points[i],nullptr,i+1);
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//再コネクト
    connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
}

void ObjectList::PullSelected(){
    //this <- model

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

void ObjectList::PushSelected(){
    //this -> model

    int count = 0;
    disconnect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//一時的にコネクト解除
    disconnect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
    this->CadModelCoreInterface::model->SelectedClear();
    for(int i=0;i<this->blocks.size();i++,count++)pushSelectedBlock(this->blocks[i],this->topLevelItem(count));
    for(int i=0;i<this->faces .size();i++,count++)pushSelectedFace (this->faces [i],this->topLevelItem(count));
    for(int i=0;i<this->edges .size();i++,count++)pushSelectedEdge (this->edges [i],this->topLevelItem(count));
    for(int i=0;i<this->points.size();i++,count++)pushSelectedPoint(this->points[i],this->topLevelItem(count));
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//再コネクト
    connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
}



void ObjectList::Delete(bool){
    for(CObject* obj : this->CadModelCoreInterface::model->GetSelected()){
        this->CadModelCoreInterface::model->Delete(obj);
    }
    this->CadModelCoreInterface::model->SelectedClear();
    UpdateObject();
}
void ObjectList::ReverseArc(bool){
    for(CObject* obj : this->CadModelCoreInterface::model->GetSelected()){
        if(obj->is<CArc>()){
            //反転
            dynamic_cast<CArc*>(obj)->SetReverse(!dynamic_cast<CArc*>(obj)->isReverse());
        }
    }
    this->CadModelCoreInterface::model->SelectedClear();
    UpdateObject();
}
void ObjectList::SetVisible(bool){
    for(CObject* obj:this->CadModelCoreInterface::model->GetSelected())obj->SetVisible(true);
}
void ObjectList::SetInvisible(bool){
    for(CObject* obj:this->CadModelCoreInterface::model->GetSelected())obj->SetVisible(false);
}
void ObjectList::SetVisibleMesh(bool){
    for(CObject* obj:this->CadModelCoreInterface::model->GetSelected()){
        if(obj->is<CBlock>()){
            dynamic_cast<CBlock*>(obj)->SetVisibleMesh(true);
        }
    }
}
void ObjectList::SetInvisibleMesh(bool){
    for(CObject* obj:this->CadModelCoreInterface::model->GetSelected()){
        if(obj->is<CBlock>()){
            dynamic_cast<CBlock*>(obj)->SetVisibleMesh(false);
        }
    }
}


ObjectList::ObjectList(QWidget *parent) :
    QTreeWidget(parent)
{
    connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
}

ObjectList::~ObjectList()
{
}
