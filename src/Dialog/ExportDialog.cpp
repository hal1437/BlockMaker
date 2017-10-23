#include "ExportDialog.h"
#include "ui_ExportDialog.h"


int ExportDialog::GetPosIndex(CPoint* p) const{
    int ans;
    QVector<CPoint*> pp = this->model->GetPoints();
    //検索
    ans = std::distance(pp.begin(),std::find(pp.begin(),pp.end(),p));
    if(ans == pp.size()){
        return -1;//pが存在しない
    }else{
        return ans;//pが存在する
    }
}

QVector<CPoint*> ExportDialog::GetBoundaryPos(CBlock* block,BoundaryDir dir)const{
    QVector<CPoint*> vertices;
    QVector<CPoint*> ans;
    for(int i=0;i<8;i++){
        vertices.push_back(block->GetPointSequence(i));
    }

    if(dir == BoundaryDir::Front){    //前面
        ans.push_back(vertices[0]);
        ans.push_back(vertices[3]);
        ans.push_back(vertices[2]);
        ans.push_back(vertices[1]);
    }else if(dir == BoundaryDir::Back){    //背面
        ans.push_back(vertices[4]);
        ans.push_back(vertices[5]);
        ans.push_back(vertices[6]);
        ans.push_back(vertices[7]);
    }else{
        int index;
        if(dir == BoundaryDir::Top   )index = 2; //上面
        if(dir == BoundaryDir::Bottom)index = 0; //下面
        if(dir == BoundaryDir::Right )index = 1; //右面
        if(dir == BoundaryDir::Left  )index = 3; //左面

        ans.push_back(vertices[(index+1)%4]);
        ans.push_back(vertices[(index+1)%4+4]);
        ans.push_back(vertices[index+4]);
        ans.push_back(vertices[index]);
    }

    return ans;
}

void ExportDialog::ChangeDirctory(){
    //ファイルパス変更ダイアログ
    QString filename = QFileDialog::getExistingDirectory(this,tr("Export"),this->ui->ExportPath->text());
    if(filename != "")this->ui->ExportPath->setText(filename);
}
void ExportDialog::Export(QString filename)const{

    //全ての辺を抽出
    QVector<CPoint*> all_points = this->model->GetPoints();
    QVector<CEdge*> all_edges  = this->model->GetEdges();

    //BlockMeshDict出力
    FoamFile file(filename+"/blockMeshDict");

    //ヘッダー出力
    file.OutHeader();

    //単位変換設定
    file.OutValue("convertToMeters",QString::number(ui->Scale->value()));

    // 頂点登録
    file.StartListDifinition("vertices");

    for(CPoint* p:this->model->GetPoints()){
        file.OutVector(*p);
    }
    file.EndScope();

    // ブロック出力
    file.StartListDifinition("blocks");
    for(CBlock* block:this->model->GetBlocks()){
        file.TabOut();
        file.OutStringInline("hex");

        //頂点番号
        QVector<int> pos_indices;

        for(int i=0;i<8;i++){
            pos_indices.push_back(IndexOf(all_points,block->GetPointSequence(i)));
        }
        file.OutVectorInline(pos_indices);

        //分割数
        QVector<int> div_indices;
        div_indices.push_back(block->GetEdgeSequence(0)->getDivide());
        div_indices.push_back(block->GetEdgeSequence(3)->getDivide());
        div_indices.push_back(block->GetEdgeSequence(8)->getDivide());
        file.OutVectorInline(div_indices);

        //分割パラメータ
        QVector<double> grading_args;

        file.OutStringInline("edgeGrading");
        for(int i=0;i<12;i++){
            qDebug() << i << block->isEdgeReverse(i);
            if(block->isEdgeReverse(i)){
                grading_args.push_back(1.0/block->GetEdgeSequence(i)->getGrading());
            }else{
                grading_args.push_back(block->GetEdgeSequence(i)->getGrading());
            }
        }
        file.OutVectorInline(grading_args);
        file.OutNewline();
    }    
    file.EndScope();

    // エッジ定義
    file.StartListDifinition("edges");
    //出力
    for(CEdge* edge:all_edges){
        file.TabOut();
        if(edge->is<CLine>()){
            file.OutStringInline("line");
            file.OutStringInline(QString::number(IndexOf(all_points,edge->start)));
            file.OutStringInline(QString::number(IndexOf(all_points,edge->end  )));
        }
        if(edge->is<CArc>()   ){
            file.OutStringInline("arc");
            file.OutStringInline(QString::number(IndexOf(all_points,edge->start)));
            file.OutStringInline(QString::number(IndexOf(all_points,edge->end  )));
            file.OutVectorInline(edge->GetMiddleDivide(0.5));
        }
        if(edge->is<CSpline>()){
            file.OutStringInline("spline");
            file.OutStringInline(QString::number(IndexOf(all_points,edge->start)));
            file.OutStringInline(QString::number(IndexOf(all_points,edge->end  )));

            file.OutStringInline("(");
            for(int i=0;i<edge->GetChildCount();i++){
                file.OutVectorInline(*edge->GetPoint(i));
            }
            file.OutStringInline(")");
        }
        file.OutNewline();
    }
    file.EndScope();


    // 境界定義
    file.StartListDifinition("boundary");
    //境界追加
    QMap<QString,std::pair<Boundary::Type,QVector<int>>> boundary_list; //(name ,[[index]])
    for(CBlock* block:this->model->GetBlocks()){
        for(int i=0;i<6;i++){
            CFace* face = block->GetFace(static_cast<BoundaryDir>(i));
            //頂点番号リスト出力
            QVector<CPoint*> vp = this->GetBoundaryPos(block,static_cast<BoundaryDir>(i));
            for(CPoint* v:vp){
                if(face->getBoundary() == Boundary::Type::none)continue;//連続は登録しない
                boundary_list[face->getName()].first = face->getBoundary();
                boundary_list[face->getName()].second.push_back(GetPosIndex(v));
            }
        }
    }
    QMap<QString,std::pair<Boundary::Type,QVector<int>>>::const_iterator it = boundary_list.constBegin();
    while (it != boundary_list.constEnd()) {

        //境界名
        file.StartDictionaryDifinition(it.key());

        //境界タイプ
        file.OutValue("type",Boundary::BoundaryTypeToString(it.value().first));

        //頂点定義
        file.StartListDifinition("faces");
        QVector<int>indices;
        for(int i=0;i<it.value().second.size();i++){
            indices.push_back(it.value().second[i]); //インデックス出力
            if((i+1) % 4 == 0){
                file.OutVector(indices);
                indices.clear();
            }
        }
        file.EndScope();
        file.EndScope();
        it++;
    }
    file.EndScope();


    //結合境界定義
    file.StartListDifinition("mergePatchPairs");
    file.EndScope();

}
void ExportDialog::AcceptDialog(){
    if(ui->ExportPath->text() != ""){
        Export(ui->ExportPath->text());
        accept();
    }
    QSettings settings("settings.ini", QSettings::IniFormat, this);
    settings.setValue("LAST_PATH",this->ui->ExportPath->text());
}

ExportDialog::ExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);
    //前回の保存先を復元
    QSettings settings("settings.ini", QSettings::IniFormat, this);
    QString str = settings.value("LAST_PATH", true).toString();
    this->ui->ExportPath->setText(str);
}

ExportDialog::~ExportDialog()
{
    delete ui;
}
