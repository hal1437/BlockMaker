#include "CFileEdge.h"


CFileEdge* CFileEdge::CreateFromFile(QString filepath){
    //念のため例外判定
    CFileEdge* edge = new CFileEdge();
    try{
        QFile file(filepath);
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);

        //最終行まで
        int rows = 1;
        while(!in.atEnd()){
            QString line = in.readLine(0);
            QStringList list = line.split(",");
            if(list.size() != 3){
                QMessageBox::critical(nullptr,
                                      tr("ファイル読み込み失敗"),
                                      tr((filepath + ":line " + QString ::number(rows) + ":列方向要素数が適切ではありません。").toStdString().c_str()));

                return nullptr;
            }
            //読み込み開始
            edge->Create(new CPoint(Pos(list[0].toDouble(),list[1].toDouble(),list[2].toDouble())));
            rows++;
        }
        edge->start_base = *edge->start;
    }catch(QException e){
       std::cerr << e.what() << std::endl;
       return nullptr;
    }
    return edge;
}


void CFileEdge::DrawGL(Pos camera,Pos center)const{
    //詳細表示に構成点の表示を含める
    for(int i =0;i<this->pos.size();i++){
        this->pos[i]->SetVisible(this->isVisibleDetail());
    }
    this->CEdge::DrawGL(camera,center);
}

CREATE_RESULT CFileEdge::Create(CPoint *pos){
    this->ObserveChild(pos);
    if(this->start == nullptr){
        this->start = pos;
    }else if(this->end == nullptr){
        this->end = pos;
    }else{
        //endを更新
        this->end->SetLock(true);//中間点はロック
        this->pos.push_back(end);
        this->end = pos;
        this->end->SetLock(true);//終点もロック
        RefreshNodes();
    }
    return CREATE_RESULT::ENDLESS;
}

CFileEdge::CFileEdge()
{

}

void CFileEdge::ChangeChildCallback(QVector<CObject *> children){
    if(exist(children,this->start)){
        Pos delta = *dynamic_cast<CPoint*>(*std::find(children.begin(),children.end(),this->start)) - this->start_base;
        for(CPoint* child : this->GetAllChildren()){
            if(child == this->start)continue;
            child->SetLock(false);
            child->MoveRelative(delta);
            child->SetLock(true);
        }
        this->start_base = *this->start;
        RefreshNodes();
    }
}
