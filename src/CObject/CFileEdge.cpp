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
        QStringList pp = filepath.split("/");
        edge->setName(pp[pp.size()-1]);
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
    this->CSpline::DrawGL(camera,center);
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
        Refresh();
    }
    return CREATE_RESULT::ENDLESS;
}

CFileEdge::CFileEdge()
{

}

CEdge* CFileEdge::Clone()const{
    CFileEdge* ptr = new CFileEdge();
    ptr->Create(this->start);
    for(CPoint* pp : this->pos){
        ptr->Create(dynamic_cast<CPoint*>(pp->Clone()));
    }
    ptr->Create(this->end);
    ptr->name    = this->name;
    ptr->grading = this->grading;
    ptr->divide  = this->divide;
    ptr->start_base = this->start_base;
    ptr->Refresh();
    return ptr;
}

void CFileEdge::ChangeChildCallback(QList<CObject *> children){
    /*
    if(exist(children,this->start)){
        this->ObserveIgnore();
        Pos delta = *dynamic_cast<CPoint*>(*std::find(children.begin(),children.end(),this->start)) - this->start_base;
        for(CPoint* child : this->GetAllChildren()){
            if(child == this->start)continue;
            child->SetLock(false);
            child->MoveRelative(delta);
            child->SetLock(true);
        }

        this->start_base = *this->start;
        this->ObserveRestart();
    }*/
    Refresh();
}

