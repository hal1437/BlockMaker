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

    if(obj->is<Restraint>()){
        return QIcon(dynamic_cast<Restraint*>(obj)->GetIconPath());
    }

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
    if(obj->is<Restraint>()){
        return dynamic_cast<Restraint*>(obj)->GetRestraintName();
    }
    return "";
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
    if(current->isSelected() && !exist(this->CadModelCoreInterface::model->GetSelected(),obj)){
        this->CadModelCoreInterface::model->AddSelected(obj);
    }
    if(!current->isSelected() && exist(this->CadModelCoreInterface::model->GetSelected(),obj)){
        this->CadModelCoreInterface::model->RemoveSelected(obj);
    }
    //子に伝達
    if(current->isExpanded()){
        for(int i=0;i<current->childCount();i++){
            PushSelectedObject(obj->GetChild(i),current->child(i));
        }
    }
}
void ObjectList::PullSelectedObject(CObject* obj,QTreeWidgetItem* current){
    current->setIcon(0,this->getIcon(obj));
    current->setSelected(exist(this->CadModelCoreInterface::model->GetSelected(),obj));
    if(current->isExpanded()){
        for(int i=0;i<current->childCount();i++){
            PullSelectedObject(obj->GetChild(i),current->child(i));
        }
    }
}

void ObjectList::SetModel(CadModelCore* m){
    this->CadModelCoreInterface::model = m;
    this->menu.SetModel(m);
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateAnyObject() ),this,SLOT(UpdateAllObject()));
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateRestraints()),this,SLOT(UpdateAllObject()));
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()  ),this,SLOT(PullSelected()));
}

void ObjectList::UpdateAllObject  (){

    //ラベルの更新
    this->points_root->setText(0,QString("点　 <") + QString::number(this->CadModelCoreInterface::model->GetPoints().size()) + ">");
    this->edges_root ->setText(0,QString("線　 <") + QString::number(this->CadModelCoreInterface::model->GetEdges() .size()) + ">");
    this->faces_root ->setText(0,QString("面　 <") + QString::number(this->CadModelCoreInterface::model->GetFaces() .size()) + ">");
    this->blocks_root->setText(0,QString("立体 <") + QString::number(this->CadModelCoreInterface::model->GetBlocks().size()) + ">");
    this->restraints_root->setText(0,QString("幾何拘束 <") + QString::number(this->CadModelCoreInterface::model->GetRestraints().size()) + ">");

    //一時的にコネクト解除
    disconnect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));
    disconnect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));

    //オブジェクトのリセット
    while(this->points_root->childCount() > 0)this->points_root->removeChild(this->points_root->child(0));
    while(this->edges_root ->childCount() > 0)this->edges_root ->removeChild(this->edges_root ->child(0));
    while(this->faces_root ->childCount() > 0)this->faces_root ->removeChild(this->faces_root ->child(0));
    while(this->blocks_root->childCount() > 0)this->blocks_root->removeChild(this->blocks_root->child(0));
    while(this->restraints_root->childCount() > 0)this->blocks_root->removeChild(this->blocks_root->child(0));

    //オブジェクトのセット
    for(int i=0;i<this->CadModelCoreInterface::model->GetPoints()    .size();i++)AddObjectToTree(this->CadModelCoreInterface::model->GetPoints()    [i],points_root    ,i+1);
    for(int i=0;i<this->CadModelCoreInterface::model->GetEdges()     .size();i++)AddObjectToTree(this->CadModelCoreInterface::model->GetEdges()     [i],edges_root     ,i+1);
    for(int i=0;i<this->CadModelCoreInterface::model->GetFaces()     .size();i++)AddObjectToTree(this->CadModelCoreInterface::model->GetFaces()     [i],faces_root     ,i+1);
    for(int i=0;i<this->CadModelCoreInterface::model->GetBlocks()    .size();i++)AddObjectToTree(this->CadModelCoreInterface::model->GetBlocks()    [i],blocks_root    ,i+1);
    for(int i=0;i<this->CadModelCoreInterface::model->GetStls()      .size();i++)AddObjectToTree(this->CadModelCoreInterface::model->GetStls()      [i],nullptr        ,i+1);
    for(int i=0;i<this->CadModelCoreInterface::model->GetRestraints().size();i++)AddObjectToTree(this->CadModelCoreInterface::model->GetRestraints()[i],restraints_root,i+1);

    //再コネクト
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));
    connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));

}

void ObjectList::PullSelected(){
    //this <- model
    disconnect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//一時的にコネクト解除
    disconnect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
    if(blocks_root->isExpanded())for(int i=0;i<this->blocks_root->childCount();i++)PullSelectedObject(this->CadModelCoreInterface::model->GetBlocks()[i],this->blocks_root->child(i));
    if(faces_root ->isExpanded())for(int i=0;i<this->faces_root ->childCount();i++)PullSelectedObject(this->CadModelCoreInterface::model->GetFaces() [i],this->faces_root ->child(i));
    if(edges_root ->isExpanded())for(int i=0;i<this->edges_root ->childCount();i++)PullSelectedObject(this->CadModelCoreInterface::model->GetEdges() [i],this->edges_root ->child(i));
    if(points_root->isExpanded())for(int i=0;i<this->points_root->childCount();i++)PullSelectedObject(this->CadModelCoreInterface::model->GetPoints()[i],this->points_root->child(i));
//    for(int i=0;i<this->CadModelCoreInterface::model->GetStls()  .size();i++,count++)PullSelectedObject(this->CadModelCoreInterface::model->GetStls()  [i],this->topLevelItem(count));
    for(int i =0;i<restraints_root->childCount();i++){
        PullSelectedObject(this->CadModelCoreInterface::model->GetRestraints()[i],restraints_root->child(i));
    }
    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//再コネクト
    connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
}

void ObjectList::PushSelected(){
    //this -> model

    disconnect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//一時的にコネクト解除
    disconnect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
    this->CadModelCoreInterface::model->SelectedClear();
    if(blocks_root->isExpanded())for(int i=0;i<this->blocks_root->childCount();i++)PushSelectedObject(this->CadModelCoreInterface::model->GetBlocks()[i],this->blocks_root->child(i));
    if(faces_root-> isExpanded())for(int i=0;i<this->faces_root ->childCount();i++)PushSelectedObject(this->CadModelCoreInterface::model->GetFaces() [i],this->faces_root ->child(i));
    if(edges_root-> isExpanded())for(int i=0;i<this->edges_root ->childCount();i++)PushSelectedObject(this->CadModelCoreInterface::model->GetEdges() [i],this->edges_root ->child(i));
    if(points_root->isExpanded())for(int i=0;i<this->points_root->childCount();i++)PushSelectedObject(this->CadModelCoreInterface::model->GetPoints()[i],this->points_root->child(i));
//    for(int i=0;i<this->CadModelCoreInterface::model->GetStls()  .size();i++)PushSelectedObject(this->CadModelCoreInterface::model->GetStls()  [i],this->stl_root  ->child(i));
    for(int i =0;i<restraints_root->childCount();i++)PushSelectedObject(this->CadModelCoreInterface::model->GetRestraints()[i],restraints_root->child(i));

    connect(this->CadModelCoreInterface::model,SIGNAL(UpdateSelected()),this,SLOT(PullSelected()));//再コネクト
    connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));
}

ObjectList::ObjectList(QWidget *parent) :
    QTreeWidget(parent)
{
    connect(this,SIGNAL(itemSelectionChanged()),this,SLOT(PushSelected()));

    //ルート作成
    points_root     = new QTreeWidgetItem(this);
    edges_root      = new QTreeWidgetItem(this);
    faces_root      = new QTreeWidgetItem(this);
    blocks_root     = new QTreeWidgetItem(this);
    restraints_root = new QTreeWidgetItem(this);
    points_root    ->setIcon(0,QIcon(":/ToolImages/Dot.png"));
    edges_root     ->setIcon(0,QIcon(":/ToolImages/Line.png"));
    faces_root     ->setIcon(0,QIcon(":/ToolImages/Face.png"));
    blocks_root    ->setIcon(0,QIcon(":/ToolImages/Blocks.png"));
    restraints_root->setIcon(0,QIcon(":/Restraint/Restraint.png"));
    this->addTopLevelItems({points_root,edges_root,faces_root,blocks_root});

}

ObjectList::~ObjectList()
{
}
