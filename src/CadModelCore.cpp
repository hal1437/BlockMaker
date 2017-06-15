#include "CadModelCore.h"


bool CadModelCore::ExportFoamFile(QString filename)const{
    std::ofstream out(filename.toStdString().c_str());
    if(!out)return false;

    //頂点リスト出力
    out << this->Points.size() << std::endl;
    for(CPoint* pos: this->Points){
        out << *pos << std::endl;
    }
    //エッジリスト出力
    out << this->Edges.size() << std::endl;
    for(CEdge* edge: this->Edges){
        std::string name;
        if(edge->is<CLine>  ())name = "CLine";
        if(edge->is<CArc>   ())name = "CArc";
        if(edge->is<CSpline>())name = "CSpline";

        out << name.c_str() << "," << IndexOf(this->Points,edge->start)
                            << "," << IndexOf(this->Points,edge->end);
        for(int i=0;i<edge->GetPosSequenceCount();i++){
            out << "," << IndexOf(this->Points,edge->GetPosSequence(i));
        }
        out << std::endl;
    }
    //面リスト出力
    out << this->Faces.size() << std::endl;
    for(CFace* face: this->Faces){
        out << "CFace";
        for(int i=0;i< face->edges.size();i++){
            out << "," << IndexOf(this->Edges,face->edges[i]);
        }
        out << std::endl;
    }
    //立体リスト出力
    out << this->Blocks.size() << std::endl;
    for(CBlock* block: this->Blocks){
        out << "CBlock";
        for(int i=0;i< block->faces.size();i++){
            out << "," << IndexOf(this->Faces,block->faces[i]);
        }
        out << std::endl;
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
/*
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
    }*/
    return false;
}

bool CadModelCore::SelectedClear(){
    this->Selected.clear();
    emit UpdateSelected();
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

