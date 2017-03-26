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
            }
        }
    }
    //Z軸方向シフトを追加
    for(VPos p:ans){
        p.z = this->ui->Width->value();
        ans.push_back(p);
    }

    return ans;
}


void ExportDialog::ChangeDirctory(){
    //ファイルパス変更ダイアログ
    QString filename = QFileDialog::getExistingDirectory(this,tr("Export"));
    if(filename != "")this->ui->ExportPath->setText(filename);
}
void ExportDialog::Export(QString filename)const{
    double width = ui->Width->value();
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
    QVector<VPos> vertices;
    out << "vertices"                       << "\n";
    out << "("                              << "\n";
    for(VPos p:this->GetVerticesPos()){
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
        //頂点番号出力
        for(int i=0;i<4;i++)out << this->GetPosIndex(VPos{block.GetVerticesPos()[i].x,block.GetVerticesPos()[i].y,0})     << " ";
        for(int i=0;i<4;i++)out << this->GetPosIndex(VPos{block.GetVerticesPos()[i].x,block.GetVerticesPos()[i].y,width}) << " ";
        out << ")";

        //分割数出力
        if(block.grading == GradingType::SimpleGrading){
            out << " (" << QString::number(block.grading_args[0]) << " "
                        << QString::number(block.grading_args[1]) << " "
                        << QString::number(block.grading_args[2]) << ")\n";
        }else if(block.grading == GradingType::EdgeGrading){
            // ?
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
    QMap<QString,QVector<int>> boundary_list;

    //境界追加
    for(CBlock block:this->blocks){
        for(int i=0;i<6;i++){
            //頂点番号リスト出力
            QVector<int> v_list;
//            for(int j=0;j<block.GetVerticesPos())

  //          boundary_list.insert(block.name[i],block.);
        }
    }

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
