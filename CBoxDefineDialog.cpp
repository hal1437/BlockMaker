#include "CBoxDefineDialog.h"
#include "ui_CBoxDefineDialog.h"
#include <QMessageBox>


void CBoxDefineDialog::paintEvent    (QPaintEvent* event){
    QPainter paint(this->ui->Viewer);

}

//面の情報を取得
BoundaryType CBoxDefineDialog::GetBoundaryType(BoundaryDir dir)const{
    QMap <QString,BoundaryType> map;
    map = {{"patch (パッチ)",Patch},
           {"wall (壁)",Patch},
           {"symmetryPlane (対称面)",Patch},
           {"cyclic (周期境界)",Patch},
           {"cyclicAMI (不整合周期境界)",Patch},
           {"wedge (2次元軸対称)",Patch},
           {"empty (2 次元)",Patch}};
    QString index;
    if     (dir == Top   )index = ui->TopTypecombo->currentText();
    else if(dir == Right )index = ui->RightTypecombo->currentText();
    else if(dir == Left  )index = ui->LeftTypecombo->currentText();
    else if(dir == Bottom)index = ui->BottomTypecombo->currentText();
    else if(dir == Front )index = ui->FrontTypecombo->currentText();
    else if(dir == Back  )index = ui->BackTypecombo->currentText();
    return map[index];
}
QString CBoxDefineDialog::GetBoundaryArgs(BoundaryDir dir)const{
    if     (dir == Top   )return (ui->TopVerticesEdit->text()    == "")? "TopBoundary"    :  ui->TopVerticesEdit->text();
    else if(dir == Right )return (ui->RightVerticesEdit->text()  == "")? "RightBoundary"  :  ui->RightVerticesEdit->text();
    else if(dir == Left  )return (ui->LeftVerticesEdit->text()   == "")? "LeftBoundary"   :  ui->LeftVerticesEdit->text();
    else if(dir == Bottom)return (ui->BottomVerticesEdit->text() == "")? "BottomBoundary" :  ui->BottomVerticesEdit->text();
    else if(dir == Front )return (ui->FrontVerticesEdit->text()  == "")? "FrontBoundary"  :  ui->FrontVerticesEdit->text();
    else if(dir == Back  )return (ui->BackVerticesEdit->text()   == "")? "BackBoundary"   :  ui->BackVerticesEdit->text();
    else return "";
}
QString CBoxDefineDialog::GetBoundaryName(BoundaryDir dir)const{
    if     (dir == Top   )return ui->TopNameEdit   ->text();
    else if(dir == Right )return ui->RightNameEdit ->text();
    else if(dir == Left  )return ui->LeftNameEdit  ->text();
    else if(dir == Bottom)return ui->BottomNameEdit->text();
    else if(dir == Front )return ui->FrontNameEdit ->text();
    else if(dir == Back  )return ui->BackNameEdit  ->text();
    else return "";
}
QString CBoxDefineDialog::GetVertices(BoundaryDir dir)const{
    if     (dir == Top   )return ui->TopVerticesEdit->text();
    else if(dir == Right )return ui->RightVerticesEdit->text();
    else if(dir == Left  )return ui->LeftVerticesEdit->text();
    else if(dir == Bottom)return ui->BottomVerticesEdit->text();
    else if(dir == Front )return ui->FrontVerticesEdit->text();
    else if(dir == Back  )return ui->BackVerticesEdit->text();
    else return "";
}

GradingType CBoxDefineDialog::GetGradigngType()const{
    GradingType type;
    if(ui->GradingCombo->currentText() == "SimpleGrading")type = SimpleGrading;
    if(ui->GradingCombo->currentText() == "EdgeGrading"  )type = EdgeGrading;
    return type;
}
QString CBoxDefineDialog::GetGradigngArgs()const{
    return ui->GradingEdit->text();
}

//エラー判定
bool CBoxDefineDialog::isFormatError()const{
    //引数の数があっているかな判定
    bool failed=false;

    int args_num = (GetGradigngType() == SimpleGrading) ? 2 : 11;
    if(GetGradigngArgs().split(' ').size() == args_num)failed = true;

    return failed;
}

//出力
CBlocks CBoxDefineDialog::ExportCBlocks()const{
    CBlocks blocks;
    for(int i=0;i<6;i++){
        BoundaryDir dir = static_cast<BoundaryDir>(i);
        blocks.boundery[i] = this->GetBoundaryType(dir);
        blocks.name[i]     = this->GetBoundaryName(dir);
        QStringList list   = this->GetVertices(dir).split(' ');
        for(int j=0;j<list.size();j++){
            blocks.vertices[j].push_back(list[j].toDouble());
        }

    }
    blocks.grading      = this->GetGradigngType();
    QStringList list = ui->GradingEdit->text().split(' ');
    for(int i=0;i<list.size();i++){
        blocks.grading_args.push_back(list[i].toDouble());
    }
    return blocks;
}

CBoxDefineDialog::CBoxDefineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CBoxDefineDialog)
{
    ui->setupUi(this);
    ui->TopVerticesEdit   ->setText("0 1 4 5");
    ui->RightVerticesEdit ->setText("1 2 5 6");
    ui->BottomVerticesEdit->setText("2 3 6 7");
    ui->LeftVerticesEdit  ->setText("3 0 7 4");
    ui->FrontVerticesEdit ->setText("0 1 2 3");
    ui->BackVerticesEdit  ->setText("4 5 6 7");
    ui->GradingEdit       ->setText("1 1 1");
}

CBoxDefineDialog::~CBoxDefineDialog()
{
    delete ui;
}

void CBoxDefineDialog::AcceptProxy(){
    if(this->isFormatError()){
        QMessageBox::information(this,"フォーマットエラー","Gradigngの引数が合っていません。",QMessageBox::Ok);
    }else{
        accept();
    }
}
