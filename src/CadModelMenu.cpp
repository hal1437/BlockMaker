#include "CadModelMenu.h"


void CadModelMenu::Show(QPoint pos){
    QVector<CObject*> selected = this->CadModelCoreInterface::model->GetSelected();
    this->menu = new QMenu();
    if(!selected.isEmpty()){
        this->delete_action          = this->menu->addAction("削除");
        connect(this->delete_action,SIGNAL(triggered(bool)),this,SLOT(Delete(bool)));
    }

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
void CadModelMenu::SetVisibleMesh(bool){
    for(CObject* obj:this->CadModelCoreInterface::model->GetSelected()){
        if(obj->is<CBlock>()){
            dynamic_cast<CBlock*>(obj)->SetVisibleMesh(true);
        }
    }
}
void CadModelMenu::SetInvisibleMesh(bool){
    for(CObject* obj:this->CadModelCoreInterface::model->GetSelected()){
        if(obj->is<CBlock>()){
            dynamic_cast<CBlock*>(obj)->SetVisibleMesh(false);
        }
    }
}

CadModelMenu::CadModelMenu(QObject *parent):
    QObject(parent)
{
}

