#include "MoveTransformDialog.h"
#include "ui_MoveTransformDialog.h"

MoveTransformDialog::MoveTransformDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MoveTransformDialog)
{
    ui->setupUi(this);
    connect(this->ui->ApplyButton    ,SIGNAL(clicked()),this,SLOT(Accept()));
    connect(this->ui->DuplicateButton,SIGNAL(clicked()),this,SLOT(Duplicate()));
    connect(this->ui->CloseButton    ,SIGNAL(clicked()),this,SLOT(close()));
    connect(this->ui->XSpinBox     ,SIGNAL(valueChanged(double)) ,this,SLOT(ValueChangedEmitter(double)));
    connect(this->ui->YSpinBox     ,SIGNAL(valueChanged(double)) ,this,SLOT(ValueChangedEmitter(double)));
    connect(this->ui->ZSpinBox     ,SIGNAL(valueChanged(double)) ,this,SLOT(ValueChangedEmitter(double)));
    connect(this->ui->AbsoluteRadio,SIGNAL(clicked(bool)) ,this,SLOT(RadioChangedEmitter(bool)));
    connect(this->ui->RelativeRadio,SIGNAL(clicked(bool)) ,this,SLOT(RadioChangedEmitter(bool)));
    connect(this              ,SIGNAL(rejected()),this,SLOT(Closed()));
    this->setWindowTitle("MoveTransform");
    this->setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
}
MoveTransformDialog::~MoveTransformDialog()
{
    delete ui;
}

void MoveTransformDialog::SetModel(CadModelCore *m){
    this->model = m;
    connect(this->model,SIGNAL(UpdateSelected()),this,SLOT(RefreshTranslated()));
}
void MoveTransformDialog::RefreshTranslated(){
    //複製
    QVector<CPoint*> points;
    QVector<CEdge* > edges;
    for(CObject* obj:this->model->GetSelected()){
        if(obj->is<CPoint>())points.append(dynamic_cast<CPoint*>(obj));
        if(obj->is<CEdge >())edges .append(dynamic_cast<CEdge*>(obj->Clone()));
        if(obj->is<CFace >()){//展開
            for(int i =0;i<obj->GetChildCount();i++){
                edges.append(dynamic_cast<CEdge*>(obj->GetChild(i)->Clone()));
            }
        }
        if(obj->is<CBlock >()){//展開
            for(int i =0;i<obj->GetChildCount();i++){
                for(int j =0;j<obj->GetChild(i)->GetChildCount();j++){
                    edges.append(dynamic_cast<CEdge*>(obj->GetChild(i)->GetChild(j)->Clone()));
                }
            }
        }
    }
    //線が含む点を吐き出す
    for(CEdge* edge:edges){
        for(int i =0;i<edge->GetChildCount();i++){
            points.append(dynamic_cast<CPoint*>(edge->GetChild(i)));
        }
    }
    //重複を削除
    unique(points);
    unique(edges);

    //マッピング
    QMap<QObject*,CPoint*> point_map;
    QVector<CPoint*> points_cloned;

    //点をクローン
    for(CPoint* pp:points){
        CPoint* cloned = dynamic_cast<CPoint*>(pp->Clone());
        points_cloned.append(cloned);
        point_map.insert(pp,cloned);
    }
    //線の子をマッピングしたものに変更
    for(CEdge* ee:edges){
        for(int i =0;i<ee->GetChildCount();i++){
            ee->SetChild(i,point_map[ee->GetChild(i)]);
        }
    }

    this->translated_pos  = points_cloned;
    this->translated_edges = edges;

    Pos value = Pos(this->ui->XSpinBox->value(),
                    this->ui->YSpinBox->value(),
                    this->ui->ZSpinBox->value());

    if(this->GetTransformMethod() == ABSOLUTE){
        this->AbsoluteMove(points_cloned.begin(),points_cloned.end(),value);
    }
    else{
        this->RelativeMove(points_cloned.begin(),points_cloned.end(),value);
    }
    emit RepaintRequest();
}

template <class Iterator> QList<CPoint*> MoveTransformDialog::ConvertChildPoint(Iterator begin,Iterator end)const{
    QList<CPoint*> ans;
    for(;begin != end;begin++){
        for(CPoint* child:(*begin)->GetAllChildren()){
            ans.push_back(child);
        }
    }
    unique(ans); //排他
    return ans;
}
MoveTransformDialog::TRANSFORM_METHOD MoveTransformDialog::GetTransformMethod()const{
    if(this->ui->RelativeRadio->isChecked())return RELATIVE;
    else return ABSOLUTE;
}

void MoveTransformDialog::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Shift){
        this->ui->XSpinBox->setValue(-this->ui->XSpinBox->value());
        this->ui->YSpinBox->setValue(-this->ui->YSpinBox->value());
        this->ui->ZSpinBox->setValue(-this->ui->ZSpinBox->value());
    }
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        this->Accept();
    }
}
void MoveTransformDialog::keyReleaseEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Shift){
        this->ui->XSpinBox->setValue(-this->ui->XSpinBox->value());
        this->ui->YSpinBox->setValue(-this->ui->YSpinBox->value());
        this->ui->ZSpinBox->setValue(-this->ui->ZSpinBox->value());
    }
}

template <class Iterator> void MoveTransformDialog::AbsoluteMove(Iterator begin,Iterator end, Pos pos){
    //選択された点の平均値を絶対移動
    QList<CPoint*> points = this->ConvertChildPoint(begin,end);

    //中心点を取得
    Pos delta;
    for(CPoint* p :points)delta += *p;
    delta /= points.size();

    //移動
    for(Iterator it = begin;it != end;it++)Pause(*it);//更新停止
    for(CPoint* p :points){
        p->MoveAbsolute(*p - delta + pos);
    }
    for(Iterator it = begin;it != end;it++)Restart(*it);//更新再開
}
template <class Iterator> void MoveTransformDialog::RelativeMove(Iterator begin,Iterator end, Pos diff){
    //選択された点を相対移動
    QList<CPoint*> points = this->ConvertChildPoint(begin,end);

    for(Iterator it=begin;it != end;it++)Pause(*it);//更新停止

    for(CPoint* p:points){
        p->MoveRelative(diff);
    }
    for(Iterator it=begin;it != end;it++)Restart(*it);//更新再開
}

void MoveTransformDialog::Pause(CObject* obj){
    obj->ObservePause();
    for(int i = 0;i<obj->GetChildCount();i++){
        Pause(obj->GetChild(i));
    }
}
void MoveTransformDialog::Restart(CObject* obj){
    obj->ObserveRestart();
    for(int i = 0;i<obj->GetChildCount();i++){
        Restart(obj->GetChild(i));
    }
}
void MoveTransformDialog::DrawTranslated(Pos camera,Pos center){
    //色を保存
    float old_color[4];
    glGetFloatv  (GL_CURRENT_COLOR,old_color);

    glColor3f(1,1,0);
    for(CObject* obj:this->translated_pos  )obj->DrawGL(camera,center);
    for(CObject* obj:this->translated_edges)obj->DrawGL(camera,center);
    //色を復元
    glColor4f(old_color[0],old_color[1],old_color[2], old_color[3]);
}
void MoveTransformDialog::ValueChangedEmitter(double){
    RefreshTranslated();
}
void MoveTransformDialog::RadioChangedEmitter(bool){
    RefreshTranslated();
}

void MoveTransformDialog::Accept(){
    //値
    Pos value = Pos(this->ui->XSpinBox->value(),
                    this->ui->YSpinBox->value(),
                    this->ui->ZSpinBox->value());
    if(this->GetTransformMethod() == ABSOLUTE) this->AbsoluteMove(this->model->GetSelected().begin(),this->model->GetSelected().end(),value);
    else                                       this->RelativeMove(this->model->GetSelected().begin(),this->model->GetSelected().end(),value);

    this->model->UpdateAnyObjectEmittor();
    RefreshTranslated();
}
void MoveTransformDialog::Duplicate(){

    //移動値
    Pos value = Pos(this->ui->XSpinBox->value(),
                    this->ui->YSpinBox->value(),
                    this->ui->ZSpinBox->value());

    QList<CPoint*> points,points_clone;//点
    QList<CEdge*>  edges ,edges_clone; //線
    QList<CFace*>  faces ,faces_clone; //面
    QList<CBlock*> blocks,blocks_clone;//立体

    //マッピング
    QMap<CObject*,CPoint*> points_map;
    QMap<CObject*,CEdge* > edges_map;
    QMap<CObject*,CFace* > faces_map;

    for(CObject* s : this->model->GetSelected()){
        //分類
        if(s->is<CPoint>())points.push_back(dynamic_cast<CPoint*>(s));
        if(s->is<CEdge> ())edges .push_back(dynamic_cast<CEdge*> (s));
        if(s->is<CFace> ())faces .push_back(dynamic_cast<CFace*> (s));
        if(s->is<CBlock>())blocks.push_back(dynamic_cast<CBlock*>(s));
    }
    //展開
    for(CBlock* bb:blocks)for(int i=0;i<bb->GetChildCount();i++)faces .append(dynamic_cast<CFace*>(bb->GetChild(i)));
    unique(faces);
    for(CFace*  ff:faces )for(int i=0;i<ff->GetChildCount();i++)edges .append(dynamic_cast<CEdge*>(ff->GetChild(i)));
    unique(edges);
    for(CEdge*  ee:edges )for(int i=0;i<ee->GetChildCount();i++)points.append(dynamic_cast<CPoint*>(ee->GetChild(i)));
    unique(points);

    //点の複製
    for(CPoint* pp:points){
        //複製
        CPoint* cloned = dynamic_cast<CPoint*>(pp->Clone());
        //マップに登録
        points_map.insert(pp,cloned);
        //リストに登録
        points_clone.append(cloned);
    }
    //線の複製
    for(CEdge* ee:edges){
        //複製
        CEdge* cloned = dynamic_cast<CEdge*>(ee->Clone());

        //マップを参照し点をすり替え
        for(int i =0;i<ee->GetChildCount();i++){
            cloned->SetChild(i,points_map[ee->GetChild(i)]);
        }
        //マップに登録
        edges_map.insert(ee,cloned);
        //リストに登録
        edges_clone.append(cloned);
    }
    //面の複製
    for(CFace* ff:faces){
        //複製
        CFace* cloned = dynamic_cast<CFace*>(ff->Clone());

        //マップを参照し点をすり替え
        for(int i =0;i<ff->GetChildCount();i++){
            cloned->SetChild(i,edges_map[ff->GetChild(i)]);
        }
        //マップに登録
        faces_map.insert(ff,cloned);
        //リストに登録
        faces_clone.append(cloned);
    }

    //面の複製
    for(CBlock* bb:blocks){
        //複製
        CBlock* cloned = dynamic_cast<CBlock*>(bb->Clone());

        //マップを参照し点をすり替え
        for(int i =0;i<bb->GetChildCount();i++){
            cloned->SetChild(i,faces_map[bb->GetChild(i)]);
        }
        //リストに登録
        blocks_clone.append(cloned);
    }

    //移動
    if(this->GetTransformMethod() == ABSOLUTE)this->AbsoluteMove(points_clone.begin(),points_clone.end(),value);
    else                                      this->RelativeMove(points_clone.begin(),points_clone.end(),value);

    this->model->AddPoints(points_clone); //モデルに追加
    this->model->AddEdges (edges_clone);  //モデルに追加
    this->model->AddFaces (faces_clone);  //モデルに追加
    this->model->AddBlocks(blocks_clone); //モデルに追加

    this->model->AutoMerge();
    this->model->UpdateAnyObjectEmittor();
}

void MoveTransformDialog::Closed(){
    //予測表示を停止
    //this->translated.clear();
    emit RepaintRequest();
}



