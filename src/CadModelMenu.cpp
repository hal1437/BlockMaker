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

CadModelMenu::CadModelMenu(QObject *parent):
    QObject(parent)
{
}

