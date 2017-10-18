#include "CStl.h"

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

    //三角形数読み込み
    std::uint32_t triangle_size;
    in.readRawData (reinterpret_cast<char*>(&triangle_size),4);

    //三角形読み込み
    for(int i =0;i< triangle_size;i++){
        float norm[3];
        float point[3][3];
        int index[3] = {-1,-1,-1};
        std::uint16_t no_used;

        //法線ベクトル
        for(int j=0;j<3;j++){
            in.readRawData (reinterpret_cast<char*>(&norm[j]),4);
        }
        //三角形座標
        for(int j=0;j<3;j++){
            for(int k=0;k<3;k++){
                in.readRawData (reinterpret_cast<char*>(&point[j][k]),4);
            }
            //構築
            Pos pp(point[j][0],point[j][1],point[j][2]);
            for(int k=0;k<ans->points.size();k++){
                if(*ans->points[k] == pp){
                    index[j] = k;
                }
            }
            if(index[j] == -1){
                index[j] = ans->points.size();
                ans->points.push_back(new CPoint(pp));
            }
        }
        //存在しなければ追加
        CEdge* ee[3] = {new CLine(ans->points[index[0]],ans->points[index[1]]),
                        new CLine(ans->points[index[1]],ans->points[index[2]]),
                        new CLine(ans->points[index[2]],ans->points[index[0]])};
        for(int j=0;j<3;j++){
            if(std::find_if(ans->edges.begin(),ans->edges.end(),[&](CEdge* e){
                return ((e->start == ee[j]->start && e->end == ee[j]->end  ) ||
                        (e->start == ee[j]->end   && e->end == ee[j]->start) );
            }) == ans->edges.end()){
                ans->edges.push_back(ee[j]);
            }
        }

        //未使用データ
        in.readRawData (reinterpret_cast<char*>(&no_used),2);
    }

    qDebug() << ans->name;
    qDebug() << "頂点数:" << ans->points.size();
    qDebug() << "エッジ数:" << ans->edges.size();

    return ans;
}

void CStl::DrawGL(Pos camera,Pos center)const{
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
