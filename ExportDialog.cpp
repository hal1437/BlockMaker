#include "ExportDialog.h"
#include "ui_ExportDialog.h"


void ExportDialog::SetBlocks(std::vector<CBlocks> blocks){
    this->blocks = blocks;
}

void ExportDialog::ChangeDirctory(){
    QString filename = QFileDialog::getSaveFileName(this,tr("Export"), "",tr("open FOAM (*.openFOAM);;All Files (*)"));
    if(filename != "")this->ui->ExportPath->setText(filename);
}
void ExportDialog::Export(QString filename)const{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(nullptr, tr("Unable to open file"),
            file.errorString());
        return;
    }
    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("UTF8"));
    //out.setVersion(QDataStream::Qt_4_5);
    //contacts.empty();   // empty existing contacts
    out << "FoamFile"                       << "\n";
    out << "{"                              << "\n";
    out << "    version     2.0;"           << "\n";
    out << "    format      ascii;"         << "\n";
    out << "    class       dictionary;"    << "\n";
    out << "    object      blockMeshDict;" << "\n";
    out << "}"                              << "\n";
    out                                     << "\n";


    out << "convertToMeters " << ui->Scale->value() << ";\n";
    out                                             << "\n";

    out << "vertices"                       << "\n";
    out << "("                              << "\n";

/*
    for(int i=0;i<objects.size();i++)this->vertices.push_back({objects[i]->x,objects[i]->y,0});
    for(int i=0;i<objects.size();i++)this->vertices.push_back({objects[i]->x,objects[i]->y,this->ui->Width->value()});
    for(int i=0;i<vertices.size();i++){
        out << "    ";
        out << "(" << vertices.x << " " << vertices.y << " "<< vertices.z << ")" << "\n";
    }
    out << ");" << "\n";

    //ブロック出力
    for
    */
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
ExportDialog::ExportDialog(std::vector<CObject *> objects, QWidget *parent) :
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
