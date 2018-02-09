#include "CadModelCore.h"


bool CadModelCore::ExportFoamFile(QString filename){

    SeqenceFileIO seq(filename);

    //CObject型出力
    std::function<void(const CObject*)> object_out = [&](const CObject* obj){
        seq.Output(obj->getName());
        seq.Output(obj->isVisible());
        seq.Output(obj->isVisibleDetail());
    };
    //CPoint型出力
    std::function<void(const CPoint*)> pos_out = [&](const CPoint* pos){
        seq.Output(pos->x());
        seq.Output(pos->y());
        seq.Output(pos->z());
        object_out(pos);
    };
    //CEdge型出力
    std::function<void(const CEdge*)> edge_out = [&](const CEdge* edge){
        if(edge->is<CLine  >())seq.OutputQString(CLine  ().DefaultClassName());
        if(edge->is<CArc   >())seq.OutputQString(CArc   ().DefaultClassName());
        if(edge->is<CSpline>())seq.OutputQString(CSpline().DefaultClassName());

        //分割情報を出力
        seq.Output(edge->getDivide());
        seq.OutputForeach(edge->getGrading().elements,[&](CEdge::Grading::GradingElement g){
            seq << g.cell;
            seq << g.dir;
            seq << g.grading;
        });

        //子の番号を出力
        seq.Output(edge->GetChildCount());
        for(CPoint* pp :edge->GetAllChildren()){
            seq << IndexOf(this->GetPoints(),pp);
        }
        object_out(edge);
    };
    //CFace型出力
    std::function<void(const CFace*)> face_out = [&](const CFace* face){

        //境界タイプ
        seq.OutputQString(face->getBoundary().name);
        seq.Output(static_cast<int>(face->getBoundary().type));

        //子の番号を出力
        seq.OutputForeach(face->edges,[&](CEdge* ee){seq << IndexOf(this->GetEdges(),ee);});

        //追加情報
        object_out(face);
    };
    //CBlock型出力
    std::function<void(const CBlock*)> block_out = [&](const CBlock* block){
        //子の番号を出力
        seq.OutputForeach(block->faces,[&](CFace* ff){seq << IndexOf(this->GetFaces(),ff);});
        //追加情報
        object_out(block);
    };

    //三平面を削除
    for(int i=0;i<3;i++)this->RemoveFaces(CFace::base[i]);

    //オブジェクト出力
    seq.OutputForeach(this->GetPoints(),pos_out);
    seq.OutputForeach(this->GetEdges (),edge_out);
    seq.OutputForeach(this->GetFaces (),face_out);
    seq.OutputForeach(this->GetBlocks(),block_out);

    //三平面を復元
    for(int i=0;i<3;i++)this->AddFaces(CFace::base[i]);

    return true;
}
bool CadModelCore::ImportFoamFile(QString filename){

    SeqenceFileIO seq(filename);

    //全て削除
    this->Points.clear();
    this->Edges.clear();
    this->Faces.clear();
    this->Blocks.clear();

    //CObject型入力
    std::function<CObject* (CObject* obj)> object_in = [&](CObject* obj){
        QString name;
        bool visible,visible_detail;
        seq.Input(name);
        seq.Input(visible);
        seq.Input(visible_detail);
        obj->setName(name);
        obj->SetVisible(visible);
        obj->SetVisibleDetail(visible_detail);
        return obj;
    };
    //CPoint型入力
    std::function<CPoint* (SeqenceFileIO&)> pos_in = [&](SeqenceFileIO& seq){
        CPoint* pos = new CPoint();
        double x,y,z;
        seq.Input(x);
        seq.Input(y);
        seq.Input(z);

        pos->x() = x;
        pos->y() = y;
        pos->z() = z;
        object_in(pos);
        return pos;
    };
    //CEdge型入力
    std::function<CEdge* (SeqenceFileIO&)> edge_in = [&](SeqenceFileIO& seq){
        CEdge* edge;
        QString class_name;
        int divide;
        CEdge::Grading gradings;
        int index_s,index_e,index_m;
        int child_size;
        seq.InputQString(class_name);

        //クラス名に応じた生成
        if(class_name == CLine  ().DefaultClassName())edge = new CLine  ();
        if(class_name == CArc   ().DefaultClassName())edge = new CArc   ();
        if(class_name == CSpline().DefaultClassName())edge = new CSpline();

        //分割情報を取得
        seq.Input(divide);
        seq.InputForeach(gradings.elements,[&](SeqenceFileIO& seq){
            CEdge::Grading::GradingElement g;
            seq >> g.cell;
            seq >> g.dir;
            seq >> g.grading;
            return g;
        });

        //子数の取得
        seq.Input(child_size);
        //始点
        seq.Input(index_s);
        edge->Create(this->GetPoints()[index_s]);

        //中央点：円弧
        if(class_name == CArc().DefaultClassName()){
            seq.Input(index_m);//始点
            dynamic_cast<CArc*>(edge)->Create(this->GetPoints()[index_m]);
            std::swap(dynamic_cast<CArc*>(edge)->start,dynamic_cast<CArc*>(edge)->center);
        }
        //補助点：スプライン
        if(class_name == CSpline().DefaultClassName()){
            for(int i =0;i<child_size-2;i++){
                seq.Input(index_m);
                dynamic_cast<CSpline*>(edge)->Create(this->GetPoints()[index_m]);
            }
        }

        //終点
        seq.Input(index_e);
        edge->Create(this->GetPoints()[index_e]);

        //値の代入
        edge->setDivide  (divide);
        edge->setGrading (gradings);

        object_in(edge);
        return edge;
    };
    //CFace型入力
    std::function<CFace* (SeqenceFileIO&)> face_in = [&](SeqenceFileIO& seq){
        CFace* face = new CFace();

        Boundary bound;
        QList<int> indexies;
        int type;
        //境界タイプ
        seq.InputQString(bound.name);
        seq.Input(type);
        bound.type = static_cast<Boundary::Type>(type);

        //子の番号入力
        seq.InputForeach(indexies,[](SeqenceFileIO& seq){
            int index;
            seq >> index;
            return index;
        });

        //値の代入
        QList<CEdge*>edges;
        for(int i:indexies){
            edges.push_back(this->GetEdges()[i]);
        }
        face->Create(edges);
        face->setBoundary(bound);

        //追加情報
        object_in(face);
        //メッシュ再生成
        face->RecalcMesh();
        return face;
    };
    //CBlock型入力
    std::function<CBlock*(SeqenceFileIO&)> block_in = [&](SeqenceFileIO& seq){
        CBlock* block = new CBlock();
        QList<int> indexies;
        //子の番号入力
        seq.InputForeach(indexies,[](SeqenceFileIO& seq){
            int index;
            seq >> index;
            return index;
        });

        //値の代入
        QList<CFace*>faces;
        for(int i:indexies){
            faces.push_back(this->GetFaces()[i]);
        }
        //追加情報
        object_in(block);

        //再構築
        block->Create(faces);
        block->Refresh();
        return block;
    };

    //オブジェクト出力
    seq.InputForeach(this->GetPoints(),pos_in);
    seq.InputForeach(this->GetEdges (),edge_in);
    seq.InputForeach(this->GetFaces (),face_in);
    seq.InputForeach(this->GetBlocks(),block_in);

    //三平面を復元
    this->ObservePause();
    for(int i=0;i<3;i++)this->AddFaces(CFace::base[i]);
    this->ObserveRestart();

    //更新
    emit UpdateAnyObjectEmittor();

    return true;
}
void CadModelCore::AddObject(CObject* obj){
    if(obj->is<CPoint>())this->AddPoints(dynamic_cast<CPoint*>(obj));
    if(obj->is<CEdge >())this->AddEdges (dynamic_cast<CEdge* >(obj));
    if(obj->is<CFace >())this->AddFaces (dynamic_cast<CFace* >(obj));
    if(obj->is<CBlock>())this->AddBlocks(dynamic_cast<CBlock*>(obj));

    //子の追加
    QList<CObject*> obj_children;
    for(int i=0;i<obj->GetChildCount();i++){
        obj_children.append(obj->GetChild(i));
    }
    if(!obj_children.empty())this->AddObjectArray(obj_children);
}
void CadModelCore::AddObjectArray(QList<CObject*> obj){
    if(obj.isEmpty())return;
    if(obj.first()->is<CPoint>())this->AddPoints(ConvertArrayType<CObject*,CPoint*>(obj));
    if(obj.first()->is<CEdge >())this->AddEdges (ConvertArrayType<CObject*,CEdge* >(obj));
    if(obj.first()->is<CFace >())this->AddFaces (ConvertArrayType<CObject*,CFace* >(obj));
    if(obj.first()->is<CBlock>())this->AddBlocks(ConvertArrayType<CObject*,CBlock*>(obj));

    QList<CObject*> obj_children;
    for(CObject* v:obj){
        for(int i=0;i<v->GetChildCount();i++){
            obj_children.append(v->GetChild(i));
        }
    }
    if(!obj_children.empty())this->AddObjectArray(obj_children);
}

void CadModelCore::Merge(QList<CPoint*> points){
    if(points.isEmpty())return ;
    CPoint* first = points.first();

    //全て先頭に結合
    for(CPoint* pos:points){
        for(CEdge* edge:this->GetEdges()){
            //posを含むエッジを探す
            for(int i=0;i<edge->GetChildCount();i++){
                //使っていれば
                if(edge->GetPoint(i) == pos){
                    edge->SetChild(i,first);//firstに上書き
                }
            }
        }
    }

    //先頭以外は消す
    points.pop_front();
    this->RemovePoints(points);
}
void CadModelCore::AutoMerge_impl(QList<CPoint*> points){

    QList<CPoint*>::iterator begin = points.begin();
    while(begin != points.end()){
        //検索
        QList<CPoint*>::iterator it_f = begin;

        QList<CPoint*> same;
        //同一の座標を探す
        while(it_f != points.end()){
            //探索
            it_f = std::find_if(it_f+1,points.end(),[&](CPoint* p){
                return (*p == **begin);
            });
            //同一の座標が存在するならば追加
            if(it_f != points.end()){
                same.push_back(*it_f);

                it_f--;//一度引き戻す
                points.erase(it_f+1);//さっきのを削除
                it_f++;//次へ
            }
        };

        //マージ
        if(same.size() > 1){
            this->Merge(same);
        }
        //次の候補へ
        begin++;
    }
}
void CadModelCore::AutoMerge(){
    this->AutoMerge_impl(this->GetPoints());
}
void CadModelCore::AutoMerge(CObject* obj){
    this->AutoMerge_impl(obj->GetAllChildren());
}
void CadModelCore::AllMerge(){
    //点
    qDebug() << "点結合中...";
    QList<CPoint*>::iterator it1_p,it2_p;
    for(it1_p = this->Points.begin();it1_p != this->Points.end();it1_p++){
        for(it2_p = it1_p+1;it2_p != this->Points.end();it2_p++){
            if(**it1_p == **it2_p && *it1_p != *it2_p){
                qDebug() << *it1_p << " <= " << *it2_p;

                //it2を含むエッジを探してit1に置き換える
                for(CEdge* edge:this->Edges){
                    for(int i =0;i<edge->GetChildCount();i++){
                        if(edge->GetChild(i) == *it2_p){
                            edge->SetChild(i,*it1_p);
                        }
                    }
                }
                it2_p--;
                this->Points.erase(it2_p+1);
            }
        }
    }
    qDebug() << "線結合中...";
    QList<CEdge*>::iterator it1_l,it2_l;
    std::function<bool(CEdge*,CEdge*)> edge_comp = [](CEdge* lhs,CEdge* rhs){
        if(lhs->GetChildCount() != rhs->GetChildCount())return false;
        for(int i =0;i< lhs->GetChildCount();i++){
            if(lhs->GetChild(i) != rhs->GetChild(i))return false;
        }
        return true;
    };
    for(it1_l = this->Edges.begin();it1_l != this->Edges.end();it1_l++){
        for(it2_l = it1_l+1;it2_l != this->Edges.end();it2_l++){
            if(edge_comp(*it1_l , *it2_l)){
                qDebug() << *it1_l << " <= " << *it2_l;

                //it2を含むエッジを探してit1に置き換える
                for(CFace* face:this->Faces){
                    for(int i =0;i<face->GetChildCount();i++){
                        if(face->GetChild(i) == *it2_l){
                            face->SetChild(i,*it1_l);
                        }
                    }
                }
                it2_l--;
                this->Edges.erase(it2_l+1);
            }
        }
    }
    qDebug() << "面結合中...";
    QList<CFace*>::iterator it1_f,it2_f;
    std::function<bool(CFace*,CFace*)> face_comp = [&](CFace* lhs,CFace* rhs){
        if(lhs->GetChildCount() != rhs->GetChildCount())return false;
        for(int i =0;i< lhs->GetChildCount();i++){
            if(lhs->GetChild(i) != rhs->GetChild(i))return false;
        }
        return true;
    };
    for(it1_f = this->Faces.begin();it1_f != this->Faces.end();it1_f++){
        for(it2_f = it1_f+1;it2_f != this->Faces.end();it2_f++){
            if(face_comp(*it1_f , *it2_f)){
                qDebug() << *it1_f << " <= " << *it2_f;

                //it2を含むエッジを探してit1に置き換える
                for(CBlock* face:this->Blocks){
                    for(int i =0;i<face->GetChildCount();i++){
                        if(face->GetChild(i) == *it2_f){
                            face->SetChild(i,*it1_f);
                        }
                    }
                }
                it2_f--;
                this->Faces.erase(it2_f+1);
            }
        }
    }

    qDebug() << "立体結合中...";
    QList<CBlock*>::iterator it1_b,it2_b;
    std::function<bool(CBlock*,CBlock*)> block_comp = [&](CBlock* lhs,CBlock* rhs){
        if(lhs->GetChildCount() != rhs->GetChildCount())return false;
        for(int i =0;i< lhs->GetChildCount();i++){
            if(lhs->GetChild(i) != rhs->GetChild(i))return false;
        }
        return true;
    };
    for(it1_b = this->Blocks.begin();it1_b != this->Blocks.end();it1_b++){
        for(it2_b = it1_b+1;it2_b != this->Blocks.end();it2_b++){
            if(block_comp(*it1_b , *it2_b)){
                qDebug() << *it1_b << " <= " << *it2_b;
                it2_b--;
                this->Blocks.erase(it2_b+1);
            }
        }
    }
    qDebug() << "結合完了";
    emit UpdateAnyObjectEmittor();
}

QList<CBlock*> CadModelCore::GetParent(CFace*  child)const{
    QList<CBlock*>ans;
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
QList<CFace*>  CadModelCore::GetParent(CEdge*  child)const{
    QList<CFace*>ans;
    for(CFace* face:this->GetFaces()){
        for(CEdge* edge : face->edges){
            if(edge == child){
                ans.push_back(face);
                break;
            }
        }
    }
    return ans;
}
QList<CEdge*>  CadModelCore::GetParent(CPoint* child)const{
    QList<CEdge*>ans;
    for(CEdge* edge:this->GetEdges()){
        if(edge->start == child || edge->end == child)ans.push_back(edge);
    }
    return ans;
}
void CadModelCore::Delete(CObject* obj){
    if(obj->is<CPoint   >())this->Delete(dynamic_cast<CPoint*   >(obj));
    if(obj->is<CEdge    >())this->Delete(dynamic_cast<CEdge*    >(obj));
    if(obj->is<CFace    >())this->Delete(dynamic_cast<CFace*    >(obj));
    if(obj->is<CBlock   >())this->Delete(dynamic_cast<CBlock*   >(obj));
    if(obj->is<CStl     >())this->Delete(dynamic_cast<CStl*     >(obj));
    if(obj->is<Restraint>())this->Delete(dynamic_cast<Restraint*>(obj));
}
void CadModelCore::Delete(CPoint* obj){
    if(obj == this->origin)return ;
    for(CEdge* parent:this->GetParent(obj))this->Delete(parent);
    this->GetPoints().removeAll(obj);
    UpdatePointsEmittor();
}
void CadModelCore::Delete(CEdge*  obj){
    for(CFace* parent:this->GetParent(obj))this->Delete(parent);
    this->GetEdges().removeAll(obj);
    UpdateEdgesEmittor();
}
void CadModelCore::Delete(CFace*  obj){
    if(exist(CFace::base,obj))return ;
    for(CBlock* parent:this->GetParent(obj))this->Delete(parent);
    this->GetFaces().removeAll(obj);
    UpdateFacesEmittor();
}
void CadModelCore::Delete(CBlock*  obj){
    this->GetBlocks().removeAll(obj);
    UpdateBlocksEmittor();
}
void CadModelCore::Delete(CStl* obj){
    this->GetStls().removeAll(obj);
    UpdateStlsEmittor();
}
void CadModelCore::Delete(Restraint* obj){
    this->GetRestraints().removeAll(obj);
    UpdateRestraintsEmittor();
}

void CadModelCore::ConflictAnyObjectEmittor(CObject* object,Conflict conf){
    emit ConflictAnyObject(object,conf);
}
void CadModelCore::SolvedAnyObjectEmittor(CObject* object){
    //if(this->isPause() == false){
        emit SolvedAnyObject(object);
    //}
}

void CadModelCore::ObservePause(){
    this->pause = true;
    for(CBlock* obj:this->Blocks)obj->ObserveStack();
    for(CFace*  obj:this->Faces )obj->ObserveStack();
    for(CEdge*  obj:this->Edges )obj->ObserveStack();
    for(CPoint* obj:this->Points)obj->ObserveStack();
}

void CadModelCore::ObserveRestart(){
    this->pause = false;
    for(CPoint* obj:this->Points)obj->ObserveRestart();
    for(CEdge*  obj:this->Edges )obj->ObserveRestart();
    for(CFace*  obj:this->Faces )obj->ObserveRestart();
    for(CBlock* obj:this->Blocks)obj->ObserveRestart();
}

void CadModelCore::SelectedClear(){
    this->Selected.clear();
    UpdateSelectedEmittor();
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
    connect(this,SIGNAL(UpdateStls  ()),this,SLOT(UpdateAnyObjectEmittor()));
    connect(this,SIGNAL(UpdateSelected     ()),this,SLOT(UpdateAnyActionEmittor()));
    connect(this,SIGNAL(UpdateAnyObject    ()),this,SLOT(UpdateAnyActionEmittor()));
    connect(this,SIGNAL(UpdateRestraints   ()),this,SLOT(UpdateAnyActionEmittor()));
    connect(this,SIGNAL(UpdateDimensions   ()),this,SLOT(UpdateAnyActionEmittor()));
}

CadModelCore::~CadModelCore()
{

}
