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

        out << name;
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
        //平面番号
        for(int i=0;i< 6;i++)out << "," << IndexOf(this->Faces,block->faces[i]);
        //境界タイプ
        for(int i=0;i< 6;i++)out << "," << block->boundery[i];
        //分割数
        for(int i=0;i< 3;i++)out << "," << block->div[i];
        //分割間隔タイプ
        out << "," << block->grading;
        //分割間隔タイプ
        for(double d:block->grading_args){
            out << "," << d;
        }
        out << std::endl;
    }
    return true;
}

bool CadModelCore::ImportFoamFile(QString filename){

    std::ifstream in(filename.toStdString().c_str());
    if(!in)return false;

    //オブジェクトを全て削除
    this->Points.clear();
    this->Edges.clear();
    this->Faces.clear();
    this->Blocks.clear();
    this->Selected.clear();

    //頂点取得
    int vertex_num;
    in >> vertex_num;
    for(int i=0;i<vertex_num;i++){
        Pos p;
        in >> p;
        this->Points.push_back(new CPoint(p));
    }

    //エッジ取得
    int edge_num;
    in >> edge_num;
    for(int i=0;i<edge_num;i++){
        std::string str;
        in >> str;

        //オブジェクト判定
        CEdge* make = nullptr;
        QStringList sl = QString(str.c_str()).split(',');

        if(sl[0] == "CLine"  )make = new CLine();
        if(sl[0] == "CArc"   )make = new CArc();
        if(sl[0] == "CSpline")make = new CSpline();

        //オブジェクト生成
        if(make != nullptr){

            if(sl[0] == "CArc"){
                //例外
                make->Create(this->Points[sl[2].toInt()]);
                make->Create(this->Points[sl[1].toInt()]);
                make->Create(this->Points[sl[3].toInt()]);
            }else{

                CREATE_RESULT res = make->Create(this->Points[sl[1].toInt()]);
                if(res == CREATE_RESULT::ENDLESS){
                    for(int j=3;j<sl.size();j++){
                        make->Create(this->Points[sl[j].toInt()]);
                    }
                    make->Create(this->Points[sl[2].toInt()]);
                }else{
                    for(int j=2;j<sl.size();j++){
                        make->Create(this->Points[sl[j].toInt()]);
                    }
                }
            }
            Edges.push_back(make);
        }
    }
    //面取得
    int face_num;
    in >> face_num;
    for(int i=0;i<face_num;i++){
        std::string str;
        in >> str;

        CFace* make = new CFace();
        QStringList sl = QString(str.c_str()).split(',');
        //エッジ判定
        for(int j = 1;j<sl.size();j++){
            make->edges.push_back(this->Edges[sl[j].toInt()]);
        }
        this->Faces.push_back(make);
    }
    //立体取得
    int block_num;
    in >> block_num;
    for(int i=0;i<block_num;i++){
        std::string str;
        in >> str;

        CBlock* make = new CBlock();
        QStringList sl = QString(str.c_str()).split(',');
        int j=1;
        //平面判定
        for(int i=0;i<6;i++,j++){
            make->faces.push_back(this->Faces[sl[j].toInt()]);
        }
        //境界タイプ
        for(int i=0;i<6;i++,j++){
            make->boundery[i] = static_cast<BoundaryType>(sl[j].toInt());
        }
        //分割数
        for(int i=0;i<3;i++,j++){
            make->div[i] = sl[j].toInt();
        }
        //分割間隔タイプ
        make->grading = static_cast<GradingType>(sl[j].toInt());
        j++;
        //分割間隔タイプ
        for(;j<sl.length();j++){
            make->grading_args.push_back(sl[j].toDouble());
        }
        make->RefreshDividePoint();
        make->ReorderEdges();
        this->Blocks.push_back(make);
    }
    emit UpdatePoints();
    emit UpdateEdges();
    emit UpdateFaces();
    emit UpdateBlocks();
    emit UpdateSelected();
    return true;
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

