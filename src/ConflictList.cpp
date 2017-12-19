#include "ConflictList.h"


void ConflictList::Repaint(){
    this->table->clear();

    for(std::pair<CObject*,Conflict> conf:this->conflicts){
        QTreeWidgetItem* item = new QTreeWidgetItem();
        //ポインタ
        item->setText(0,QString("0x%1").arg((quintptr)conf.first,QT_POINTER_SIZE * 2, 16, QChar('0')));
        //エラー原因
        item->setText(1,conf.second.error);

        //解決策を追加
        for(ConflictSolver* solver:conf.second.solvers){
            QTreeWidgetItem* item_s = new QTreeWidgetItem();
            item->addChild(item_s);
            //解決策説明
            item_s->setText(0,solver->explain);

            //解決ボタン
            QPushButton* button = new QPushButton("Solved!");
            connect(button,SIGNAL(pressed()),solver,SLOT(Run()));
            this->table->setItemWidget(item_s,1,button);
        }
        //ツリーに追加
        this->table->addTopLevelItem(item);
    }
    emit Changed();
}

//モデルの競合を自動的に追加
void ConflictList::SetModel(CadModelCore *m){
    this->model = m;
    connect(this->model,SIGNAL(ConflictAnyObject(CObject*,Conflict)),this,SLOT(AddConflict(CObject*,Conflict)));
    connect(this->model,SIGNAL(SolvedAnyObject  (CObject*)         ),this,SLOT(RemoveConflict(CObject*)));
    Repaint();
}

void ConflictList::AddConflict(CObject *obj, Conflict conf){
    //配列に存在しなければ追加
    if(!exist_if(this->conflicts,[&](std::pair<CObject*,Conflict> v){return v.first == obj;})){
        this->conflicts.push_back(std::make_pair(obj,conf));
    }
    Repaint();
}
void ConflictList::RemoveConflict(CObject *obj){
    //配列から全て削除
    std::sort(this->conflicts.begin(),this->conflicts.end());
    this->conflicts.erase(std::remove_if(this->conflicts.begin(),
                                         this->conflicts.end(),
                                         [&](std::pair<CObject*,Conflict> v){return v.first == obj;}),
                          this->conflicts.end());
    Repaint();
}

//テーブルセット
void ConflictList::SetWidget(QTreeWidget* table){
    this->table = table;
}



ConflictList::ConflictList()
{

}
