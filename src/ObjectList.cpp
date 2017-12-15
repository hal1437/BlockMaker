#include "ObjectList.h"

QIcon ObjectList::getIcon(CObject *obj){
    QString filepath;
    if(obj==nullptr)return QIcon();
    if(obj->is<CBlock>())filepath = ":/ToolImages/Blocks";
    if(obj->is<CFace> ())filepath = ":/ToolImages/Face";
    if(obj->is<CEdge>()){
        if(obj->is<CLine>  ())filepath = ":/ToolImages/Line";
        if(obj->is<CArc>   ())filepath = ":/ToolImages/Arc";
        if(obj->is<CSpline>())filepath = ":/ToolImages/Spline";
    }
    if(obj->is<CPoint>())filepath = ":/ToolImages/Dot";
    if(obj->is<CStl  >())filepath = ":/ToolImages/Stl";

    //詳細表示
    if(!obj->is<CPoint>() && obj->isVisibleDetail())filepath += "Mesh";

    //不可視は薄くする
    if(obj->isVisible())filepath += ".png";
    else                filepath += "_disabled.png";

    return QIcon(filepath);
}
QString ObjectList::getName(CObject *obj){
    if(obj->is<CBlock>())return "Block";
    if(obj->is<CFace> ())return "Face";
    if(obj->is<CEdge>()){
        if(obj->is<CLine>  ())return "Line";
        if(obj->is<CArc>   ())return "Arc";
        if(obj->is<CSpline>())return "Spline";
    }
    if(obj->is<CPoint>())return "Point";
    if(obj->is<CStl  >())return "STL";
}

void ObjectList::mouseReleaseEvent(QMouseEvent* event){
    //メニュー表示
    if(event->button() == Qt::RightButton){
        menu.Show(event->globalPos());
    }
}

void ObjectList::AddObjectToTree(CObject* obj,QTreeWidgetItem* parent,int index){
    QTreeWidgetItem* item = new QTreeWidgetItem();

    if(obj == nullptr){
        //不明なオブジェクト
        item->setText(0,"nullptr");
    }else{

        //名前
        if(obj->getName() == ""){
            item->setText(0,this->getName(obj) + QString::number(index));
        }else{
            item->setText(0,obj->getName());
        }

        //選択状態とアイコン反映
        item->setIcon(0,getIcon(obj));
        item->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),obj));
    }

    //トップならTopLevelに追加
    if(parent == nullptr)this->addTopLevelItem(item);
    else                 parent->addChild(item);

    //例外:CSTLは子を追加しない
    //例外:CFace::baseは子を追加しない
    //例外:nullptrは子を追加しない
    if(!obj->is<CStl>() && !exist(CFace::base,obj) && obj != nullptr){
        //子を登録
        for(int i = 0;i<obj->GetChildCount();i++){
            AddObjectToTree(obj->GetChild(i),item,i+1);
        }
    }
}
void ObjectList::PushSelectedObject(CObject* obj,QTreeWidgetItem* current){
    current->setIcon(0,this->getIcon(obj));
    if(current->isSelected())this->CadModelCoreInterface::model->AddSelected(obj);
    else                     this->CadModelCoreInterface::model->RemoveSelected(obj);
    for(int i=0;i<current->childCount();i++){
        PushSelectedObject(obj->GetChild(i),current->child(i));
    }
}
void ObjectList::PullSelectedObject(CObject* obj,QTreeWidgetItem* current){
    current->setIcon(0,this->getIcon(obj));
    current->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),obj));
    for(int i=0;i<current->childCount();i++){
        PullSelectedObject(obj->GetChild(i),current->child(i));
    }
}

void ObjectList::SetModel(CadModelCore* m){
    this->CadModelCoreInterface::model = m;
    this->menu.SetModel(m);
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateAnyObject() ),this,SLOT(UpdateObject()));
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateRestraints()),this,SLOT(UpdateObject()));
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()  ),this,SLOT(PullSelected()));
}

void ObjectList::UpdateObject  (){

    //再コピー
    this->points = this->CadModelCoreInterface::model->GetPoints();
    this->edges  = this->CadModelCoreInterface::model->GetEdges ();
    this->faces  = this->CadModelCoreInterface::model->GetFaces ();
    this->blocks = this->CadModelCoreInterface::model->GetBlocks();
    this->stls   = this->CadModelCoreInterface::model->GetStls  ();

    //親オブジェクトのみを残す。
    for(CEdge*  edge  : this->edges )for(CPoint* p : edge ->GetAllChildren())this->points.removeAll(p);
    for(CFace*  face  : this->faces )for(CEdge*  e : face ->edges           )this->edges .removeAll(e);
    for(CBlock* block : this->blocks)for(CFace*  f : block->faces           )this->faces .removeAll(f);
    for(CStl*   stl   : this->stls  )for(CPoint* p : stl  ->points          )this->points.removeAll(p);

    //ツリーにセット
    //一時的にコネクト解除
    disconnect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));
    disconnect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
    this->clear();

    //オブジェクトのセット
    for(int i=0;i<this->stls  .size();i++)AddObjectToTree(this->stls  [i],nullptr,i+1);
    for(int i=0;i<this->blocks.size();i++)AddObjectToTree(this->blocks[i],nullptr,i+1);
    for(int i=0;i<this->faces .size();i++)AddObjectToTree(this->faces [i],nullptr,i+1);
    for(int i=0;i<this->edges .size();i++)AddObjectToTree(this->edges [i],nullptr,i+1);
    for(int i=0;i<this->points.size();i++)AddObjectToTree(this->points[i],nullptr,i+1);

    //拘束のセット
    QTreeWidgetItem* rest_root = new QTreeWidgetItem();
    rest_root->setText(0,"幾何拘束");
    rest_root->setIcon(0,QIcon(":/Restraint/Restraint.png"));
    for(Restraint* rest : this->CadModelCoreInterface::model->GetRestraints()){
        QTreeWidgetItem* rest_node = new QTreeWidgetItem();
        rest_node->setText(0,rest->GetRestraintName());
        rest_node->setIcon(0,QIcon(rest->GetIconPath()));
        rest_root->addChild(rest_node);
    }
    this->addTopLevelItem(rest_root);
    //再コネクト
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));
    connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
}

void ObjectList::PullSelected(){
    //this <- model
    int count = 0;
    disconnect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//一時的にコネクト解除
    disconnect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
    for(int i=0;i<this->stls  .size();i++,count++)PullSelectedObject(this->stls  [i],this->topLevelItem(count));
    for(int i=0;i<this->blocks.size();i++,count++)PullSelectedObject(this->blocks[i],this->topLevelItem(count));
    for(int i=0;i<this->faces .size();i++,count++)PullSelectedObject(this->faces [i],this->topLevelItem(count));
    for(int i=0;i<this->edges .size();i++,count++)PullSelectedObject(this->edges [i],this->topLevelItem(count));
    for(int i=0;i<this->points.size();i++,count++)PullSelectedObject(this->points[i],this->topLevelItem(count));
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//再コネクト
    connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
}

void ObjectList::PushSelected(){
    //this -> model

    int count = 0;
    disconnect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//一時的にコネクト解除
    disconnect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
    this->CadModelCoreInterface::model->SelectedClear();
    for(int i=0;i<this->stls  .size();i++,count++)PushSelectedObject(this->stls  [i],this->topLevelItem(count));
    for(int i=0;i<this->blocks.size();i++,count++)PushSelectedObject(this->blocks[i],this->topLevelItem(count));
    for(int i=0;i<this->faces .size();i++,count++)PushSelectedObject(this->faces [i],this->topLevelItem(count));
    for(int i=0;i<this->edges .size();i++,count++)PushSelectedObject(this->edges [i],this->topLevelItem(count));
    for(int i=0;i<this->points.size();i++,count++)PushSelectedObject(this->points[i],this->topLevelItem(count));
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
