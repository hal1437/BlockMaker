#include "CadModelCore.h"


bool CadModelCore::ExportFoamFile(QString filename)const{
    std::ofstream out(filename.toStdString().c_str());
    if(!out)return false;

    QVector<Pos> points;//全ての頂点
    for(CEdge* edge : Edges){
        points.push_back(*edge->start);
        points.push_back(*edge->end);
        for(int j=0;j<edge->GetMiddleCount();j++){
            points.push_back(*edge->GetMiddle(j));
        }
    }

    //並び替え&同一排除
    std::sort(points.begin(),points.end());
    points.erase(std::unique(points.begin(),points.end()),points.end());

    //オブジェクトリスト作成
    QVector<std::pair<QString,QVector<int>>> pairs;//(名前,頂点番号)
    for(int i =0;i<Edges.size();i++){
        QString name = "Unknown";
        QVector<int> vertex;
        if(Edges[i]->is<CPoint>()){
            vertex.push_back(IndexOf(points,*dynamic_cast<CPoint*>(Edges[i])));
            name = "CPoint";
        }
        if(Edges[i]->is<CEdge>()){
            if(Edges[i]->is<CLine>  ())name = "CLine";
            if(Edges[i]->is<CArc>   ())name = "CArc";
            if(Edges[i]->is<CSpline>())name = "CSpline";

            //始点終点を追加
            vertex.push_back(IndexOf(points,*Edges[i]->start));
            vertex.push_back(IndexOf(points,*Edges[i]->end));
            //中継点を追加
            for(int j=0;j<Edges[i]->GetMiddleCount();j++){
                vertex.push_back(IndexOf(points,*Edges[i]->GetMiddle(j)));
            }
        }
        //登録
        pairs.push_back(std::make_pair(name,vertex));
    }

    //頂点数
    out << points.size() << std::endl;
    //頂点
    for(Pos p : points){
        out << p << std::endl;
    }

    //オブジェクト数
    out << pairs.size() << std::endl;
    //オブジェクト
    for(std::pair<QString,QVector<int>> p : pairs){
        out << p.first.toStdString();
        for(int i=0;i<p.second.size();i++){
            out << "," << p.second[i];
        }
        out << std::endl;
    }

    //ブロック
    out << Blocks.size() << std::endl;
    for(CBlock* b : Blocks){
        out << b->depth << std::endl;
        out << b->grading << std::endl;

        for(double d : b->grading_args)out << d << " ";
        out << std::endl;
        for(BoundaryType t : b->boundery)out << t << " ";
        out << std::endl;
        for(QString n : b->name)out << n.toStdString() << " ";
        out << std::endl;
        for(int d : b->div)out << d << " ";
        out << std::endl;

        for(int i=0;i<4;i++){
            for(int j=0;j<Edges.size();j++){
                if(Edges[j] == b->GetEdge(i)){
                    out << j << " ";
                    break;
                }
            }
        }
    }
    return true;
}

bool CadModelCore::ImportFoamFile(QString filename){

    std::ifstream in(filename.toStdString().c_str());
    if(!in)return false;

    //オブジェクトをクリア
    this->Edges.clear();

    //頂点数取得
    int vertex_num;
    in >> vertex_num;

    //頂点取得
    QVector<CPoint*> points;
    for(int i=0;i<vertex_num;i++){
        Pos p;
        in >> p;
        if(p != Pos()){
            points.push_back(new CPoint(p));
        }else{
            points.push_back(this->origin);
        }
    }

    //オブジェクト数取得
    int object_num;
    in >> object_num;

    //オブジェクト復元
    for(int i=0;i<object_num;i++){
        std::string str;
        in >> str;

        //オブジェクト判定
        CEdge* make = nullptr;
        QStringList sl = QString(str.c_str()).split(',');

        //if(sl[0] == "CPoint" )make = new CPoint();
        if(sl[0] == "CLine"  )make = new CLine();
        if(sl[0] == "CArc"   )make = new CArc();
        if(sl[0] == "CSpline")make = new CSpline();
        //オブジェクト生成
        if(make != nullptr){

            CREATE_RESULT res = make->Create(points[sl[1].toInt()]);

            if(res == CREATE_RESULT::ENDLESS){
                for(int j=3;j<sl.size();j++){
                    make->Create(points[sl[j].toInt()]);
                }
                make->Create(points[sl[2].toInt()]);
            }else{
                for(int j=2;j<sl.size();j++){
                    make->Create(points[sl[j].toInt()]);
                }
            }
            Edges.push_back(make);
        }
    }

    int block_num;
    in >> block_num;
    for(int i=0;i<block_num;i++){
        CBlock* block = new CBlock();
        int g;
        in >> block->depth;
        in >> g;
        block->grading = static_cast<GradingType>(g);

        if(block->grading == GradingType::SimpleGrading)block->grading_args.resize(3);
        else                                           block->grading_args.resize(12);

        for(int j=0;j<block->grading_args.size();j++){
            in >> block->grading_args[j];
        }
        for(int j=0;j<6;j++){
            int b;
            in >> b;
            block->boundery[j] = static_cast<BoundaryType>(b);
        }
        for(int j=0;j<6;j++){
            std::string str;
            in >> str;
            block->name[j] = str.c_str();
        }
        for(int j=0;j<3;j++){
            in >> block->div[j];
        }
        QVector<CEdge*> ed;
        for(int i=0;i<4;i++){
            int j;
            in >> j;
            ed.push_back(Edges[j]);
        }
        block->SetEdgeAll(ed);
        this->AddBlocks(block);
    }
    return false;
}


CadModelCore::CadModelCore(QWidget *parent):
    QObject(parent)
{
    this->origin = new CPoint(Pos(0,0,0));
    this->origin->ControlPoint(true);
}

CadModelCore::~CadModelCore()
{

}

