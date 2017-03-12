#include "ExportDialog.h"
#include "ui_ExportDialog.h"


void ExportDialog::SetBlocks(QVector<CBlock> blocks){
    this->blocks = blocks;
}

void ExportDialog::ChangeDirctory(){
    QString filename = QFileDialog::getExistingDirectory(this,tr("Export"));
    if(filename != "")this->ui->ExportPath->setText(filename);
}
void ExportDialog::Export(QString filename)const{
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
    QVector<VPos> vertices;
    for(CBlock block:this->blocks){
        QVector<Pos> pp = block.GetVerticesPos();
        for(Pos p:pp)vertices.push_back(VPos{p.x,p.y,0});                        //構成点を登録
        for(Pos p:pp)vertices.push_back(VPos{p.x,p.y,this->ui->Width->value()}); //奥行き方向移動構成点を登録
    }
    out << "vertices"                       << "\n";
    out << "("                              << "\n";
    for(VPos p:vertices){
        out << "\t(" << p.x << " " << p.y << " " << p.z << ")\n";
    }
    out << ");"                              << "\n";


    //
    // ブロック出力
    //
    out << "blocks"                       << "\n";
    out << "("                            << "\n";
    for(CBlock block:this->blocks){

        out << "\t" << "hex(";
        //indexリストを作成
        int index[4];
        QVector<Pos> b_vertices = block.GetVerticesPos();
        for(int i=0;i<4;i++){
            index[i] = std::distance(vertices.begin(),std::find_if(vertices.begin(),vertices.end(),[&](VPos p){
                return (p.x == b_vertices[i].x && p.y == b_vertices[i].y,p.z == 0);
            }));
        }
        //頂点番号出力
        for(int i=0;i<4;i++)out << index[i] << " ";
        for(int i=0;i<4;i++)out << index[i] + vertices.size() / 2 << " ";
        out << ")";

        //分割数出力
        if(block.grading == GradingType::SimpleGrading){
            out << " SimpleGrading (" ;
            for(int i=0;i<3;i++){
                out << block.grading_args[i];
                if(i != 2){
                    out << " "; //最後以外ならばスペース
                }
            }
            out << ")\n" ;
        }else if(block.grading == GradingType::EdgeGrading){

        }
    }    
    out << ");" << "\n";

    //
    // エッジ定義
    //
    out << "edges" << "\n";
    out << "("     << "\n";
    out << ");"    << "\n";

    //
    // 境界定義
    //
    out << "boundary" << "\n";
    out << "("        << "\n";

    out << ");"       << "\n";


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
