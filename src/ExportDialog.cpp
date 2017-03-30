#include "ExportDialog.h"
#include "ui_ExportDialog.h"


void ExportDialog::SetBlocks(QVector<CBlock> blocks){
    this->blocks = blocks;
}

int ExportDialog::GetPosIndex(VPos p) const{
    int ans;
    QVector<VPos> pp = this->GetVerticesPos();
    //検索
    ans = std::distance(pp.begin(),std::find(pp.begin(),pp.end(),p));
    if(ans == pp.size()){
        return -1;//pが存在しない
    }else{
        return ans;//pが存在する
    }
}
QVector<VPos> ExportDialog::GetVerticesPos() const{
    //全頂点リスト作成(重複無し)
    QVector<VPos> ans;
    for(CBlock block:blocks){
        for(Pos pos:block.GetVerticesPos()){
            //配列内に存在していなければ
            if(!exist(ans,VPos{pos.x,pos.y,0})){
                ans.push_back(VPos{pos.x,pos.y,0});
                ans.push_back(VPos{pos.x,pos.y,block.depth});
            }
        }
    }

    //並び替え
    std::sort(ans.begin(),ans.end(),[](VPos lhs,VPos rhs){
        //return std::tie(lhs.z,) > std::tie(lhs.z);
        return true;
    });

    return ans;
}

QVector<VPos> ExportDialog::GetBoundaryPos(CBlock block,BoundaryDir dir)const{
    QVector<Pos> vertices = block.GetVerticesPos();
    QVector<VPos> ans;

    if(dir == BoundaryDir::Front){    //前面
        for(Pos p:vertices){
            ans.push_back(VPos{p.x,p.y,0});
        }
    }else if(dir == BoundaryDir::Back){    //背面
        for(Pos p:vertices){
            ans.push_back(VPos{p.x,p.y,block.depth});
        }
    }else{
        if(dir == BoundaryDir::Top   )std::sort(vertices.begin(),vertices.end(),[](Pos lhs,Pos rhs){return lhs.y > rhs.y;}); //上面
        if(dir == BoundaryDir::Bottom)std::sort(vertices.begin(),vertices.end(),[](Pos lhs,Pos rhs){return lhs.y < rhs.y;}); //下面
        if(dir == BoundaryDir::Right )std::sort(vertices.begin(),vertices.end(),[](Pos lhs,Pos rhs){return lhs.x > rhs.x;}); //右面
        if(dir == BoundaryDir::Left  )std::sort(vertices.begin(),vertices.end(),[](Pos lhs,Pos rhs){return lhs.x < rhs.x;}); //左面

        //ソート結果の1番2番のみ抽出
        ans.push_back(VPos{vertices[0].x,vertices[0].y,0});
        ans.push_back(VPos{vertices[0].x,vertices[0].y,block.depth});
        ans.push_back(VPos{vertices[1].x,vertices[1].y,block.depth});
        ans.push_back(VPos{vertices[1].x,vertices[1].y,0});
    }

    return ans;
}

void ExportDialog::ChangeDirctory(){
    //ファイルパス変更ダイアログ
    QString filename = QFileDialog::getExistingDirectory(this,tr("Export"));
    if(filename != "")this->ui->ExportPath->setText(filename);
}
void ExportDialog::Export(QString filename)const{
    const char* tab = "    ";
    //出力
    QFile file(filename + "/blockMeshDict");
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(nullptr, tr("ファイルが開けません"),file.errorString());
        return;
    }

    //文字コードUTF8
    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("UTF8"));

    //out.setVersion(QDataStream::Qt_4_5);
    //contacts.empty();   // empty existing contacts


    //
    // ヘッダー書き込み
    //
    out << "FoamFile"                       << "\n";
    out << "{"                              << "\n";
    out << "    version     2.0;"           << "\n";
    out << "    format      ascii;"         << "\n";
    out << "    class       dictionary;"    << "\n";
    out << "    object      blockMeshDict;" << "\n";
    out << "}"                              << "\n";
    out                                     << "\n";
    //単位変換設定
    out << "convertToMeters " << ui->Scale->value() << ";" << "\n";
    out                                                    << "\n";


    //
    // 頂点登録
    //
    out << "vertices" << "\n";
    out << "("        << "\n";
    for(VPos p:this->GetVerticesPos()){
        out << tab << "(" << p.x << " " << p.y << " " << p.z << ")\n";
    }
    out << ");"       << "\n";
    out               << "\n";


    //
    // ブロック出力
    //
    out << "blocks" << "\n";
    out << "("      << "\n";
    for(CBlock block:this->blocks){

        out << tab << "hex (";
        //頂点番号出力
        for(int i=0;i<4;i++)out << this->GetPosIndex(VPos{block.GetVerticesPos()[i].x,block.GetVerticesPos()[i].y,0})     << " ";
        for(int i=0;i<4;i++){
            out << this->GetPosIndex(VPos{block.GetVerticesPos()[i].x,block.GetVerticesPos()[i].y,block.depth});
            if(i!=3)out << " ";
        };
        out << ") ";

        //分割数出力
        for(int i=0;i<3;i++){
            if(i == 0) out << "(";
            else       out << " ";
            out << QString::number(block.div[i]);
        }
        out << ") ";

        //分割パラメータ出力
        if(block.grading == GradingType::SimpleGrading){
            out << "simpleGrading ";
            for(int i=0;i<3;i++){
                if(i == 0) out << "(";
                else       out << " ";
                out << QString::number(block.grading_args[i]);
            }
            out << ")\n";
        }else if(block.grading == GradingType::EdgeGrading){
            out << "edgeGrading";
            for(int i=0;i<12;i++){
                if(i == 0) out << "(";
                else       out << " ";
                out << QString::number(block.grading_args[i]);
            }
            out << ")\n";
        }
    }    
    out << ");" << "\n";
    out         << "\n";

    //
    // エッジ定義
    //
    out << "edges" << "\n";
    out << "("     << "\n";
    out << ");"    << "\n";


    //
    // 境界定義
    //

    //境界追加
    QMap<QString,std::pair<BoundaryType,QVector<int>>> boundary_list; //(name ,[[index]])
    for(CBlock block:this->blocks){
        for(int i=0;i<6;i++){
            //頂点番号リスト出力
            QVector<VPos> vp = this->GetBoundaryPos(block,static_cast<BoundaryDir>(i));
            for(VPos v:vp){
                boundary_list[block.name[i]].first = block.boundery[i];
                boundary_list[block.name[i]].second.push_back(GetPosIndex(v));
            }
        }
    }

    out << "boundary" << "\n";
    out << "("        << "\n";
    QMap<QString,std::pair<BoundaryType,QVector<int>>>::const_iterator it = boundary_list.constBegin();
    while (it != boundary_list.constEnd()) {
        //境界名
        out << tab << it.key() << "\n";
        out << tab <<"{\n";

        //境界タイプ
        if(it.value().first == BoundaryType::Patch        ) out << tab << tab << "type patch;\n";
        if(it.value().first == BoundaryType::Wall         ) out << tab << tab << "type wall;\n";
        if(it.value().first == BoundaryType::SymmetryPlane) out << tab << tab << "type symmetryPlane;\n";
        if(it.value().first == BoundaryType::Cyclic       ) out << tab << tab << "type cyclic;\n";
        if(it.value().first == BoundaryType::CyclicAMI    ) out << tab << tab << "type cyclicAMI;\n";
        if(it.value().first == BoundaryType::Wedge        ) out << tab << tab << "type wedge;\n";
        if(it.value().first == BoundaryType::Empty        ) out << tab << tab << "type empty;\n";

        //頂点定義
        out << tab << tab << "faces\n";
        out << tab << tab << "(\n";
        for(int i=0;i<it.value().second.size();i++){
            if(i % 4 == 0){
                out << tab << tab << tab << "("; //先端文字列
            }else{
                out << " ";       //中間文字列
            }
            out << it.value().second[i]; //インデックス出力

            if((i+1) % 4 == 0){
                out << ")\n";     //終端文字列
            }
        }
        out << tab << tab << ");\n";
        out << tab << "}\n";
        it++;
    }

    out << ");"       << "\n";



    out << "mergePatchPairs\n";
    out << "(\n";

    out << ");\n";

}
void ExportDialog::AcceptDialog(){
    if(ui->ExportPath->text() != ""){
        Export(ui->ExportPath->text());
        accept();
    }
}

ExportDialog::ExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);
}
ExportDialog::ExportDialog(QVector<CObject *> objects, QWidget *parent) :
    QDialog(parent),
    objects(objects),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);
}

ExportDialog::~ExportDialog()
{
    delete ui;
}
