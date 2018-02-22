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

        //制御点を非表示
        for(int i =0;i<edge->pos.size();i++){
            edge->pos[i]->SetVisible(false);
        }
    }catch(QException e){
       std::cerr << e.what() << std::endl;
       return nullptr;
    }
    return edge;
}

CFileEdge::CFileEdge()
{

}


