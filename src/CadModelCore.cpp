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
        for(int i=0;i<edge->GetChildCount();i++){
            out << "," << IndexOf(this->Points,edge->GetPoint(i));
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
        for(int i=0;i< 6;i++)out << "," << block->GetFaceFormDir(static_cast<BoundaryDir>(i))->boundary;
        //分割数
        for(int i=0;i< 3;i++)out << "," << block->div[i];
        //分割間隔タイプ
        //out << "," << block->grading;
        //分割間隔タイプ
//        for(double d:block->grading_args){
//            out << "," << d;
//        }
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
            CFace* face = make->GetFace(static_cast<BoundaryDir>(i));
            face->boundary = static_cast<BoundaryType>(sl[j].toInt());
        }
        //分割数
        for(int i=0;i<3;i++,j++){
            make->div[i] = sl[j].toInt();
        }
        //分割間隔タイプ
        //make->grading = static_cast<GradingType>(sl[j].toInt());
        j++;
        //分割間隔タイプ
        for(;j<sl.length();j++){
            //make->grading_args.push_back(sl[j].toDouble());
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


void CadModelCore::AddObject(CObject* obj){
    if(obj->is<CPoint>())this->AddPoints(dynamic_cast<CPoint*>(obj));
    if(obj->is<CEdge >())this->AddEdges (dynamic_cast<CEdge* >(obj));
    if(obj->is<CFace >())this->AddFaces (dynamic_cast<CFace* >(obj));
    if(obj->is<CBlock>())this->AddBlocks(dynamic_cast<CBlock*>(obj));
    for(int i=0;i<obj->GetChildCount();i++){
        this->AddObject(obj->GetChild(i));
    }
}
void CadModelCore::Merge(QVector<CPoint*> points){
    if(points.isEmpty())return ;
    CPoint* first = points.first();
    //全て先頭に結合
    for(int i=1;i<points.size();i++){
        for(CEdge* edge:this->GetEdges()){
            for(int j=0;j<edge->GetChildCount();j++){
                if(edge->GetPoint(j) == points[i]){
                    CPoint* old = edge->GetPoint(j);
                    edge->SetChild(j,first);
                    this->RemovePoints(old);
                }
            }
        }
    }
}

void CadModelCore::AutoMerge_impl(QVector<CPoint*> points){
    for(int i=0;i<points.size();i++){
        QVector<CPoint*> same;
        same.push_back(points[i]);
        for(int j=0;j<points.size();j++){
            if(points[i] != points[j] && *points[i] == *points[j]){
                same.push_back(points[j]);
                points.removeAll(points[j]);
                j--;
            }
        }
        //マージ
        if(same.size() > 1){
            this->Merge(same);
            i = 0;
        }
    }
}

void CadModelCore::AutoMerge(){
    this->AutoMerge_impl(this->GetPoints());
}
void CadModelCore::AutoMerge(CObject* obj){
    this->AutoMerge_impl(obj->GetAllChildren());
}


QVector<CBlock*> CadModelCore::GetParent(CFace*  child)const{
    QVector<CBlock*>ans;
    for(CBlock* block:this->GetBlocks()){
        for(CFace* face : block->faces){
            if(face == child){
                ans.push_back(block);
                break;
            }
        }
    }
    return ans;
}
QVector<CFace*>  CadModelCore::GetParent(CEdge*  child)const{
    QVector<CFace*>ans;
    for(CFace* face:this->GetFaces()){
        for(CEdge* edge : face->edges){
            if(edge == child)ans.push_back(face);
            break;
        }
    }
    return ans;
}
QVector<CEdge*>  CadModelCore::GetParent(CPoint* child)const{
    QVector<CEdge*>ans;
    for(CEdge* edge:this->GetEdges()){
        if(edge->start == child || edge->end == child)ans.push_back(edge);
    }
    return ans;
}
void CadModelCore::Delete(CBlock*  obj){
    this->GetBlocks().removeAll(obj);
    emit UpdateBlocks();
}
void CadModelCore::Delete(CFace*  obj){
    for(CBlock* parent:this->GetParent(obj))this->Delete(parent);
    this->GetFaces().removeAll(obj);
    emit UpdateFaces();
}
void  CadModelCore::Delete(CEdge*  obj){
    for(CFace* parent:this->GetParent(obj))this->Delete(parent);
    this->GetEdges().removeAll(obj);
    emit UpdateEdges();
}
void  CadModelCore::Delete(CPoint* obj){
    for(CEdge* parent:this->GetParent(obj))this->Delete(parent);
    this->GetPoints().removeAll(obj);
    emit UpdatePoints();
}
void CadModelCore::Delete(CObject* obj){
    if(obj->is<CPoint>())this->Delete(dynamic_cast<CPoint*>(obj));
    if(obj->is<CEdge> ())this->Delete(dynamic_cast<CEdge* >(obj));
    if(obj->is<CFace> ())this->Delete(dynamic_cast<CFace* >(obj));
    if(obj->is<CBlock>())this->Delete(dynamic_cast<CBlock*>(obj));
}



void CadModelCore::SelectedClear(){
    this->Selected.clear();
    emit UpdateSelected();
}

void CadModelCore::UpdateObject(){
    emit UpdateAnyObject();
}

void CadModelCore::UpdateAction(){
    emit UpdateAnyAction();
}


CadModelCore::CadModelCore(QWidget *parent):
    QObject(parent)
{
    this->origin = new CPoint(Pos(0,0,0));
    this->origin->SetControlPoint(true);
    connect(this,SIGNAL(UpdatePoints()),this,SLOT(UpdateObject()));
    connect(this,SIGNAL(UpdateEdges ()),this,SLOT(UpdateObject()));
    connect(this,SIGNAL(UpdateFaces ()),this,SLOT(UpdateObject()));
    connect(this,SIGNAL(UpdateBlocks()),this,SLOT(UpdateObject()));
    connect(this,SIGNAL(UpdateSelected     ()),this,SLOT(UpdateAction()));
    connect(this,SIGNAL(SelectObjectChanged()),this,SLOT(UpdateAction()));
    connect(this,SIGNAL(UpdateAnyObject    ()),this,SLOT(UpdateAction()));
    connect(this,SIGNAL(UpdateRestraints   ()),this,SLOT(UpdateAction()));
    connect(this,SIGNAL(UpdateRestraints   ()),this,SLOT(UpdateAction()));
    connect(this,SIGNAL(UpdateDimensions   ()),this,SLOT(UpdateAction()));
}

CadModelCore::~CadModelCore()
{

}

