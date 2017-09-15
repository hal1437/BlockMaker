#include "CadModelCore.h"


bool CadModelCore::ExportFoamFile(QString filename)const{
    std::ofstream out(filename.toStdString().c_str());
    if(!out)return false;

    //バージョン出力
    out << 3 << std::endl;

    //頂点リスト出力
    out << this->Points.size() << std::endl;
    for(CPoint* pos: this->Points){
        //座標
        out << pos->getName().toStdString() << ",";
        out << pos->x() << "," ;
        out << pos->y() << "," ;
        out << pos->z() << std::endl;
    }

    //エッジリスト出力
    out << this->Edges.size() << std::endl;
    for(CEdge* edge: this->Edges){

        //エッジタイプ
        std::string name;
        if(edge->is<CLine>  ())name = "CLine";
        if(edge->is<CArc>   ())name = "CArc";
        if(edge->is<CSpline>())name = "CSpline";
        out << name;

        //オブジェクト名
        out << "," << edge->getName().toStdString().c_str();


        //構成点インデックス
        for(int i=0;i<edge->GetChildCount();i++){
            out << "," << IndexOf(this->Points,edge->GetPoint(i));
        }
        //分割数
        out << "," << edge->getDivide();
        //エッジ寄せ係数
        out << "," << edge->getGrading();
        //詳細表示
        out << "," << edge->isVisibleDetail();
        //改行
        out << std::endl;
    }

    //面リスト出力
    out << this->Faces.size()-3 << std::endl;
    for(CFace* face: this->Faces){
        //三平面は出力しない
        if(exist(CFace::base,face))continue;

        //面タイプ
        out << "CFace";
        //オブジェクト名
        out << "," << face->getName().toStdString().c_str();

        //構成線インデックス
        for(int i=0;i< face->edges.size();i++){
            out << "," << IndexOf(this->Edges,face->edges[i]);
        }
        //境界名
        out << "," << face->getName().toStdString();
        //境界タイプ
        out << "," << Boundary::BoundaryTypeToString(face->getBoundary()).toStdString();
        //詳細表示
        out << "," << face->isVisibleDetail();
        //改行
        out << std::endl;
    }
    //立体リスト出力
    out << this->Blocks.size() << std::endl;
    for(CBlock* block: this->Blocks){
        //立体タイプ
        out << "CBlock";
        //オブジェクト名
        out << "," << block->getName().toStdString().c_str();

        //平面インデックス
        for(int i=0;i< 6;i++)out << "," << IndexOf(this->Faces,block->faces[i])-3;
        //分割数
        //for(int i=0;i< 3;i++)out << "," << block->div[i];
        //詳細表示
        out << "," << block->isVisibleDetail();

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
    this->AddPoints(CPoint::origin);//原点

    //バージョン取得
    int version;
    in >> version;

    //頂点リスト取得
    int vertex_num;
    in >> vertex_num;
    for(int i=0;i<vertex_num;i++){
        std::string str;
        in >> str;
        QStringList sl = QString(str.c_str()).split(',');
        if(sl[0] == "原点")continue;
        CPoint* p = new CPoint();
        //名前
        p->setName(sl[0]);
        //座標
        p->x() = sl[1].toDouble();
        p->y() = sl[2].toDouble();
        p->z() = sl[3].toDouble();
        //モデルに追加
        this->Points.push_back(p);
    }

    //エッジリスト取得
    int edge_num;
    in >> edge_num;
    for(int i=0;i<edge_num;i++){
        std::string str;
        //エッジタイプ
        CEdge* make = nullptr;
        in >> str;
        QStringList sl = QString(str.c_str()).split(',');
        if(sl[0] == "CLine"  )make = new CLine();
        if(sl[0] == "CArc"   )make = new CArc();
        if(sl[0] == "CSpline")make = new CSpline();

        //名前
        make->setName(sl[1]);

        //オブジェクト生成
        if(make != nullptr){

            if(make->is<CArc>()){
                //例外
                make->Create(this->Points[sl[3].toInt()]);
                make->Create(this->Points[sl[2].toInt()]);
                make->Create(this->Points[sl[4].toInt()]);
            }else{

                CREATE_RESULT res = make->Create(this->Points[sl[2].toInt()]);
                if(res == CREATE_RESULT::ENDLESS){
                    //spline
                    for(int j=3;j<sl.size()-3;j++){
                        make->Create(this->Points[sl[j].toInt()]);
                    }
                    make->Create(this->Points[sl[3].toInt()]);
                }else{
                    //line
                    for(int j=3;j<sl.size()-3;j++){
                        make->Create(this->Points[sl[j].toInt()]);
                    }
                }
            }
            //分割数取得
            make->setDivide(sl[sl.size()-3].toInt());
            //分割数取得
            make->setGrading(sl[sl.size()-2].toDouble());
            //詳細表示取得
            make->SetVisibleDetail(sl[sl.size()-1]=="1");
            //モデルに追加
            Edges.push_back(make);
        }
    }
    //面取得
    int face_num;
    in >> face_num;
    for(int i=0;i<face_num;i++){
        //面タイプ
        std::string str;
        in >> str;

        //エッジ取得
        CFace* make = new CFace();
        QStringList sl = QString(str.c_str()).split(',');

        //名前
        make->setName(sl[1]);

        QVector<CEdge*> ee;
        for(int j = 2;j<sl.size()-3;j++){
            ee.push_back(this->Edges[sl[j].toInt()]);
        }
        make->Create(ee);//作成

        //境界名
        make->setName(sl[sl.size()-3]);
        //境界名
        make->setBoundary(Boundary::StringToBoundaryType(sl[sl.size()-2]));
        //詳細表示取得
        make->SetVisibleDetail(sl[sl.size()-1]=="1");

        //モデルに追加
        this->Faces.push_back(make);
    }
    //立体取得
    int block_num;
    in >> block_num;
    for(int i=0;i<block_num;i++){
        std::string str;
        in >> str;

        //平面判定
        CBlock* make = new CBlock();
        QStringList sl = QString(str.c_str()).split(',');
        //名前
        make->setName(sl[1]);

        int j=2;
        QVector<CFace*> faces;
        for(int i=0;i<6;i++,j++){
            faces.push_back(this->Faces[sl[j].toInt()]);
        }
        //詳細表示取得
        make->SetVisibleDetail(sl[sl.size()-1]=="1");
        make->Create(faces);
        //モデルに追加
        this->Blocks.push_back(make);
    }
    //更新
    emit UpdatePoints();
    emit UpdateEdges();
    emit UpdateFaces();
    emit UpdateBlocks();
    emit UpdateSelected();

    //三平面復元
    for(int i=0;i<3;i++){
        this->AddFaces(CFace::base[i]);//原点
    }
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
void CadModelCore::Delete(CEdge*  obj){
    for(CFace* parent:this->GetParent(obj))this->Delete(parent);
    this->GetEdges().removeAll(obj);
    emit UpdateEdges();
}
void CadModelCore::Delete(CPoint* obj){
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
void CadModelCore::Delete(Restraint* obj){
    this->Restraints.removeAll(obj);
    emit UpdateRestraints();
}

void CadModelCore::UpdatePause(){
}

void CadModelCore::UpdateRestart(){
}

void CadModelCore::SelectedClear(){
    this->Selected.clear();
    emit UpdateSelected();
}

CadModelCore::CadModelCore(QWidget *parent):
    QObject(parent)
{
    this->origin = new CPoint(Pos(0,0,0));
    this->origin->SetControlPoint(true);
    connect(this,SIGNAL(UpdatePoints()),this,SLOT(UpdateAnyObjectEmittor()));
    connect(this,SIGNAL(UpdateEdges ()),this,SLOT(UpdateAnyObjectEmittor()));
    connect(this,SIGNAL(UpdateFaces ()),this,SLOT(UpdateAnyObjectEmittor()));
    connect(this,SIGNAL(UpdateBlocks()),this,SLOT(UpdateAnyObjectEmittor()));
    connect(this,SIGNAL(UpdateSelected     ()),this,SLOT(UpdateAnyActionEmittor()));
    connect(this,SIGNAL(SelectObjectChanged()),this,SLOT(UpdateAnyActionEmittor()));
    connect(this,SIGNAL(UpdateAnyObject    ()),this,SLOT(UpdateAnyActionEmittor()));
    connect(this,SIGNAL(UpdateRestraints   ()),this,SLOT(UpdateAnyActionEmittor()));
    connect(this,SIGNAL(UpdateDimensions   ()),this,SLOT(UpdateAnyActionEmittor()));
}

CadModelCore::~CadModelCore()
{

}

