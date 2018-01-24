#include "CadModelMenu.h"


void CadModelMenu::Show(QPoint pos){
    QList<CObject*> selected = this->CadModelCoreInterface::model->GetSelected();
    this->menu = new QMenu();

    //全てのオブジェクト
    if(selected.size() > 0 ){
        this->delete_action          = this->menu->addAction("削除");
        connect(this->delete_action,SIGNAL(triggered(bool)),this,SLOT(Delete(bool)));
    }

    //いずれかが円弧
    if(std::any_of(selected.begin(),selected.end(),[&](CObject* obj){return obj->is<CArc>();})){
        this->reverse_action         = this->menu->addAction("円弧反転");
        connect(this->reverse_action   ,SIGNAL(triggered(bool)),this,SLOT(ReverseArc(bool)));
    }
    //いずれかが「非表示」
    if(std::any_of(selected.begin(),selected.end(),[&](CObject* obj){return !obj->isVisible();})){
        this->visible_action         = this->menu->addAction("表示");
        connect(this->visible_action        ,SIGNAL(triggered(bool)),this,SLOT(SetVisible(bool)));
    }
    //いずれかが「表示」
    if(std::any_of(selected.begin(),selected.end(),[&](CObject* obj){return obj->isVisible();})){
        this->invisible_action       = this->menu->addAction("非表示");
        connect(this->invisible_action      ,SIGNAL(triggered(bool)),this,SLOT(SetInvisible(bool)));
    }
    //いずれかが「詳細非表示」
    if(std::any_of(selected.begin(),selected.end(),[&](CObject* obj){return !obj->isVisibleDetail();})){
        this->visible_mesh_action   = this->menu->addAction("詳細表示");
        connect(this->visible_mesh_action  ,SIGNAL(triggered(bool)),this,SLOT(SetVisibleDetail(bool)));
    }
    //いずれかが「詳細表示」
    if(std::any_of(selected.begin(),selected.end(),[&](CObject* obj){return obj->isVisibleDetail();})){
        this->invisible_mesh_action = this->menu->addAction("詳細非表示");
        connect(this->invisible_mesh_action,SIGNAL(triggered(bool)),this,SLOT(SetInVisibleDetail(bool)));
    }
    //いずれかが「詳細表示」
    if(std::any_of(selected.begin(),selected.end(),[&](CObject* obj){return obj->isVisibleDetail();})){
        this->invisible_mesh_action = this->menu->addAction("詳細非表示");
        connect(this->invisible_mesh_action,SIGNAL(triggered(bool)),this,SLOT(SetInVisibleDetail(bool)));
    }
    //選択されたオブジェクトが線のみかつ2個以上
    if(std::all_of  (selected.begin(),selected.end(),[&](CObject* obj){return obj->is<CEdge>();}) &&
       selected.size()  >= 2){
        this->invisible_mesh_action = this->menu->addAction("線を結合");
        connect(this->invisible_mesh_action,SIGNAL(triggered(bool)),this,SLOT(MergeEdge(bool)));
    }
    //選択されたオブジェクトが点である、かつ一つの線上に存在する。
    if(selected.size() ==1 && std::count_if(selected.begin(),selected.end(),[&](CObject* obj){return obj->is<CPoint>();}) == 1){
        CPoint* pos = dynamic_cast<CPoint*>(selected.first());
        if(std::count_if(this->model->GetEdges().begin(),this->model->GetEdges().end(),[&](CEdge* edge){
            return edge->isOnEdge(*pos);
        }) == 1){
            this->invisible_mesh_action = this->menu->addAction("線を分割");
            connect(this->invisible_mesh_action,SIGNAL(triggered(bool)),this,SLOT(DivideEdge(bool)));
        }
    }


    this->menu->exec(pos);
}

void CadModelMenu::Delete(bool){
    for(CObject* obj : this->CadModelCoreInterface::model->GetSelected()){
        this->CadModelCoreInterface::model->Delete(obj);
    }
    this->CadModelCoreInterface::model->SelectedClear();
}
void CadModelMenu::ReverseArc(bool){
    for(CObject* obj : this->CadModelCoreInterface::model->GetSelected()){
        if(obj->is<CArc>()){
            //反転
            dynamic_cast<CArc*>(obj)->SetReverse(!dynamic_cast<CArc*>(obj)->isReverse());
        }
    }
    this->CadModelCoreInterface::model->SelectedClear();
}
void CadModelMenu::SetVisible(bool){
    for(CObject* obj:this->CadModelCoreInterface::model->GetSelected())obj->SetVisible(true);
}
void CadModelMenu::SetInvisible(bool){
    for(CObject* obj:this->CadModelCoreInterface::model->GetSelected())obj->SetVisible(false);
}
void CadModelMenu::SetVisibleDetail(bool){
    for(CObject* obj:this->CadModelCoreInterface::model->GetSelected()){
        obj->SetVisibleDetail(true);
    }
}
void CadModelMenu::SetInVisibleDetail(bool){
    for(CObject* obj:this->CadModelCoreInterface::model->GetSelected()){
        obj->SetVisibleDetail(false);
    }
}
void CadModelMenu::DivideEdge(bool){
    CPoint* pos = dynamic_cast<CPoint*>(this->model->GetSelected().first());
    CEdge* edge = *std::find_if(this->model->GetEdges().begin(),this->model->GetEdges().end(),[&](CEdge* edge){
        return edge->isOnEdge(*pos);
    });

    //線を分割しモデルに追加
    QList<CObject*> objects = ConvertArrayType<CEdge*,CObject*>(edge->DivideEdge(pos));
    if(!objects.empty()){
        this->model->AddObjectArray(objects);
        this->model->RemoveEdges(edge);
    }
}
void CadModelMenu::MergeEdge(bool){
    QList<CEdge*> edges = ConvertArrayType<CObject*,CEdge*>(this->model->GetSelected());
    QList<CEdge*> merges;

    //線を分割しモデルに追加
    for(int i =0;i<edges.size()-1;){
         CEdge* merged = edges[i]->MergeEdge(edges[i+1]);
         if(merged != nullptr){
             //成功
             merges.append(merged);
             merges.removeAll(edges[i]);
             this->model->RemoveEdges(edges[i]);
             this->model->RemoveEdges(edges[i+1]);
             edges[i] = merged;
             edges.removeAt(i+1);
         }else{
             //失敗
             i++;
         }
    }
    this->model->AddEdges(merges);
}


CadModelMenu::CadModelMenu(QObject *parent):
    QObject(parent)
{
}

