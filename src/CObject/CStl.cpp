#include "CStl.h"

CStl* CStl::AddTriangle(CStl* stl,Pos pos[3]){
    //点の追加
    int index[3] = {-1,-1,-1};//posがpoint内に存在する時のインデックス
    for(int i=0;i<3;i++){
        //点の重複チェック
        for(int j=0;j<stl->points.size();j++){
            if(*stl->points[j] == pos[i]){
                index[i] = j;
            }
        }
        //なければ追加
        if(index[i] == -1){
            index[i] = stl->points.size();
            stl->points.push_back(new CPoint(pos[i]));
            stl->points.back()->SetLock(true);
        }
    }
    //存在しなければ追加
    QVector<CEdge*> ee = {new CLine(stl->points[index[0]],stl->points[index[1]]),
                          new CLine(stl->points[index[1]],stl->points[index[2]]),
                          new CLine(stl->points[index[2]],stl->points[index[0]])};
    for(int j=0;j<3;j++){
        if(std::find_if(stl->edges.begin(),stl->edges.end(),[&](CEdge* e){
            return ((e->start == ee[j]->start && e->end == ee[j]->end  ) ||
                    (e->start == ee[j]->end   && e->end == ee[j]->start) );
        }) == stl->edges.end()){
            stl->edges.push_back(ee[j]);
        }
    }
    //面の追加
    CFace* face = new CFace();
    face->Create(ee);
    stl->faces.push_back(face);
}

CStl* CStl::CreateFromFile(QString filepath){
    CStl* ans = new CStl();
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly))return nullptr;

    char name[80];

    QDataStream in(&file);

    //名前読み込み
    in.readRawData ( name , 80 );
    ans->name = name;
    ans->name.resize(80);

    //テキスト
    if(std::find(name,name+80,'\n') != name+80){
        qDebug() << "テキスト";
        QTextStream t_in(&file);
        t_in.seek(0);
        while(!t_in.atEnd()){
            QString str = t_in.readLine();
            QStringList list = str.split(' ');
            int erase_index;
            for(erase_index=0;list[erase_index] == "";erase_index++);
            if(erase_index > 0)list.erase(list.begin(),list.begin()+erase_index);
            //名前
            if(list[0] == "solid"){
                ans->name = list[1];
            }
            //法線ベクトル
            if(list[0] == "facet"){
                //無視
            }
            //座標点
            if(list[0] == "outer"){
                Pos pos[3];
                for(int i=0;i<3;i++){
                    QString str_s = t_in.readLine();
                    QStringList list_s = str_s.split(' ');
                    int erase_index;
                    for(erase_index=0;list_s[erase_index] == "";erase_index++);
                    if(erase_index > 0)list_s.erase(list_s.begin(),list_s.begin()+erase_index);

                    pos[i] = Pos(list_s[1].toDouble(),list_s[2].toDouble(),list_s[3].toDouble());
                }
                AddTriangle(ans,pos);
            }
        }

    }else{
        qDebug() << "バイナリ";
        //三角形数読み込み
        std::uint32_t triangle_size;
        in.readRawData (reinterpret_cast<char*>(&triangle_size),4);

        //三角形読み込み
        for(int i =0;i< triangle_size;i++){
            float norm[3];
            float point[3][3];
            std::uint16_t no_used;

            //法線ベクトル
            for(int j=0;j<3;j++){
                in.readRawData (reinterpret_cast<char*>(&norm[j]),4);
            }
            //三角形座標
            Pos pos[3];
            for(int j=0;j<3;j++){
                for(int k=0;k<3;k++){
                    in.readRawData (reinterpret_cast<char*>(&point[j][k]),4);
                }
                //構築
                pos[j] = Pos(point[j][0],point[j][1],point[j][2]);
            }
            AddTriangle(ans,pos);

            //未使用データ
            in.readRawData(reinterpret_cast<char*>(&no_used),2);
        }
    }

    qDebug() << ans->name;
    qDebug() << "頂点数:" << ans->points.size();
    qDebug() << "エッジ数:" << ans->edges.size();

    return ans;
}

void CStl::DrawGL(Pos camera,Pos center)const{
    for(int i=0;i<this->edges.size();i++){
        this->edges[i]->DrawGL(camera,center);
    }
}

//中間点操作
CObject* CStl::GetChild(int index){
    return this->edges[index];
}
void     CStl::SetChild(int index,CObject* obj){
    this->edges[index] = dynamic_cast<CEdge*>(obj);
}
int      CStl::GetChildCount()const{
    return this->edges.size();
}

//近接点
Pos CStl::GetNearPos (const Pos& )const{
    return Pos();
}
Pos CStl::GetNearLine(const Pos& ,const Pos& )const{
    return Pos();
}

//複製
CObject* CStl::Clone()const{
    CStl* dup = new CStl();
    dup->points = this->points;
    dup->edges  = this->edges;
    return dup;
}



CStl::CStl()
{

}
