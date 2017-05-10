#include "ExportDialog.h"
#include "ui_ExportDialog.h"


void ExportDialog::SetBlocks(QVector<CBlock> blocks){
    this->blocks = blocks;
}

int ExportDialog::GetPosIndex(Pos p) const{
    int ans;
    QVector<Pos> pp = this->GetVerticesPos();
    //検索
    ans = std::distance(pp.begin(),std::find(pp.begin(),pp.end(),p));
    if(ans == pp.size()){
        return -1;//pが存在しない
    }else{
        return ans;//pが存在する
    }
}
QVector<Pos> ExportDialog::GetVerticesPos() const{
    //全頂点リスト作成(重複無し)
    QVector<Pos> ans;
    for(CBlock block:blocks){
        for(Pos pos:block.GetVerticesPos()){
            //配列内に存在していなければ
            if(!exist(ans,pos)){
                ans.push_back(pos);
            }
        }
    }
    //Z方向シフト分を作成
    for(CBlock block:blocks){
        for(Pos pos:block.GetVerticesPos()){
            //配列内に存在していなければ
            Pos next = pos + Pos(0,0,block.depth);
            if(!exist(ans,next)){
                ans.push_back(next);
            }
        }
    }

    return ans;
}

QVector<Pos> ExportDialog::GetBoundaryPos(CBlock block,BoundaryDir dir)const{
    QVector<Pos> vertices;
    QVector<Pos> ans;
    vertices.resize(8);
    for(int i=0;i<4;i++){
        vertices[i]   = block.GetVerticesPos()[i];
        vertices[i+4] = block.GetVerticesPos()[i] + Pos(0,0,block.depth);
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

    QVector<CEdge*> edges;

    //全ての辺を抽出
    for(CBlock block : this->blocks){
        for(int i=0;i<4;i++){
            edges.push_back(block.GetEdge(i));
            edges.push_back(block.GetEdge(i)->Clone());//複製
            *edges.back()->start += Pos(0,0,block.depth);
            *edges.back()->end   += Pos(0,0,block.depth);
            for(int j=0;j<edges.back()->GetMiddleCount();j++){
                *edges.back()->GetMiddle(j) += Pos(0,0,block.depth);
            }
        }
    }

    //BlockMeshDict出力
    FoamFile file(filename+"/blockMeshDict");

    //ヘッダー出力
    file.OutHeader();

    //単位変換設定
    file.OutValue("convertToMeters",QString::number(ui->Scale->value()));

    // 頂点登録
    file.StartListDifinition("vertices");
    for(Pos p:this->GetVerticesPos()){
        file.OutVector(p);
    }
    file.EndScope();

    // ブロック出力
    file.StartListDifinition("blocks");
    for(CBlock block:this->blocks){
        file.TabOut();
        file.OutStringInline("hex");

        //頂点番号
        QVector<int> pos_indices;
        for(int i=0;i<4;i++){
            Pos p = block.GetClockworksPos(i);
            pos_indices.push_back(this->GetPosIndex(p));
        }
        for(int i=0;i<4;i++){
            Pos p = block.GetClockworksPos(i);
            pos_indices.push_back(this->GetPosIndex(p + Pos(0,0,block.depth)));
        };
        file.OutVectorInline(pos_indices);

        //分割数
        QVector<int> div_indices;
        for(int i=0;i<3;i++){
            div_indices.push_back(block.div[i]);
        }
        file.OutVectorInline(div_indices);

        //分割パラメータ
        QVector<double> grading_args;
        if(block.grading == GradingType::SimpleGrading){
            file.OutStringInline("simpleGrading");
            for(int i=0;i<3;i++){
                grading_args.push_back(block.grading_args[i]);
            }
            file.OutVectorInline(grading_args);
        }else if(block.grading == GradingType::EdgeGrading){
            file.OutStringInline("edgeGrading");
            for(int i=0;i<12;i++){
                grading_args.push_back(block.grading_args[i]);
            }
            file.OutVectorInline(grading_args);
        }
        file.OutNewline();
    }    
    file.EndScope();

    // エッジ定義
    file.StartListDifinition("edges");
    //出力
    for(CEdge* edge:edges){
        file.TabOut();
        if(edge->is<CLine>()  ){
            file.OutStringInline("line");
            file.OutStringInline(QString::number(this->GetPosIndex(*edge->start)));
            file.OutStringInline(QString::number(this->GetPosIndex(*edge->end  )));
        }
        if(edge->is<CArc>()   ){
            file.OutStringInline("arc");
            file.OutStringInline(QString::number(this->GetPosIndex(*edge->start)));
            file.OutStringInline(QString::number(this->GetPosIndex(*edge->end  )));
            file.OutVectorInline(*edge->GetMiddle(0));
        }
        if(edge->is<CSpline>()){
            file.OutStringInline("spline");
            file.OutStringInline(QString::number(this->GetPosIndex(*edge->start)));
            file.OutStringInline(QString::number(this->GetPosIndex(*edge->end  )));

            file.OutStringInline("(");
            for(int i=0;i<edge->GetMiddleCount();i++){
                file.OutVectorInline(*edge->GetMiddle(i));
            }
            file.OutStringInline(")");
        }
        file.OutNewline();
    }
    file.EndScope();


    // 境界定義
    file.StartListDifinition("boundary");
    //境界追加
    QMap<QString,std::pair<BoundaryType,QVector<int>>> boundary_list; //(name ,[[index]])
    for(CBlock block:this->blocks){
        for(int i=0;i<6;i++){
            //頂点番号リスト出力
            QVector<Pos> vp = this->GetBoundaryPos(block,static_cast<BoundaryDir>(i));
            for(Pos v:vp){
                if(block.boundery[i] == BoundaryType::None)continue;//連続は登録しない
                boundary_list[block.name[i]].first = block.boundery[i];
                boundary_list[block.name[i]].second.push_back(GetPosIndex(v));
            }
        }
    }
    QMap<QString,std::pair<BoundaryType,QVector<int>>>::const_iterator it = boundary_list.constBegin();
    while (it != boundary_list.constEnd()) {

        //境界名
        file.StartDictionaryDifinition(it.key());

        //境界タイプ
        if(it.value().first == BoundaryType::Patch        ) file.OutValue("type","patch");
        if(it.value().first == BoundaryType::Wall         ) file.OutValue("type","wall");
        if(it.value().first == BoundaryType::SymmetryPlane) file.OutValue("type","symmetryPlane");
        if(it.value().first == BoundaryType::Cyclic       ) file.OutValue("type","cyclic");
        if(it.value().first == BoundaryType::CyclicAMI    ) file.OutValue("type","cyclicAMI");
        if(it.value().first == BoundaryType::Wedge        ) file.OutValue("type","wedge");
        if(it.value().first == BoundaryType::Empty        ) file.OutValue("type","empty");

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
