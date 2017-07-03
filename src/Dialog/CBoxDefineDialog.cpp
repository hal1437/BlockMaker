#include "CBoxDefineDialog.h"
#include "ui_CBoxDefineDialog.h"
#include <QMessageBox>

QComboBox* CBoxDefineDialog::ConvertDirToCombo   (BoundaryDir dir)const{
    if     (dir == Top   )return ui->TopTypecombo;
    else if(dir == Right )return ui->RightTypecombo;
    else if(dir == Left  )return ui->LeftTypecombo;
    else if(dir == Bottom)return ui->BottomTypecombo;
    else if(dir == Front )return ui->FrontTypecombo;
    else if(dir == Back  )return ui->BackTypecombo;
    return nullptr;
}
QLineEdit* CBoxDefineDialog::ConvertDirToNameEdit(BoundaryDir dir)const{
    if     (dir == Top   )return ui->TopNameEdit;
    else if(dir == Right )return ui->RightNameEdit;
    else if(dir == Left  )return ui->LeftNameEdit;
    else if(dir == Bottom)return ui->BottomNameEdit;
    else if(dir == Front )return ui->FrontNameEdit;
    else if(dir == Back  )return ui->BackNameEdit;
    return nullptr;
}
QString      CBoxDefineDialog::ConvertBoundaryToString(BoundaryType type)const{
    if     (type == BoundaryType::Patch        )return QString("patch (パッチ)");
    else if(type == BoundaryType::Wall         )return QString("wall (壁)");
    else if(type == BoundaryType::SymmetryPlane)return QString("symmetryPlane (対称面)");
    else if(type == BoundaryType::Cyclic       )return QString("cyclic (周期境界)");
    else if(type == BoundaryType::CyclicAMI    )return QString("cyclicAMI (不整合周期境界)");
    else if(type == BoundaryType::Wedge        )return QString("wedge (2次元軸対称)");
    else if(type == BoundaryType::Empty        )return QString("empty (2次元)");
    else if(type == BoundaryType::None         )return QString("None(連続)");
    else return QString("Unknown");
}
BoundaryType CBoxDefineDialog::ConvertStringToBoundary(QString str)const{
    for(int i=0;i<8;i++){
        if(str == ConvertBoundaryToString(static_cast<BoundaryType>(i))){
            return static_cast<BoundaryType>(i);
        }
    }
    return static_cast<BoundaryType>(0);
}
QString      CBoxDefineDialog::ConvertGradingToString (GradingType  dir)const{
    if     (dir == GradingType::SimpleGrading)return QString("SimpleGrading");
    else if(dir == GradingType::EdgeGrading  )return QString("EdgeGrading");
    else return QString("");
}
GradingType  CBoxDefineDialog::ConvertStringToGrading (QString str)const{
    if     (str == "SimpleGrading")return GradingType::SimpleGrading;
    else if(str == "EdgeGrading"  )return GradingType::EdgeGrading;
    else return GradingType::EmptyGrading;
}

//面の情報を取得
BoundaryType CBoxDefineDialog::GetBoundaryType(BoundaryDir dir)const{
    QString index;
    if     (dir == Top   )index = ui->TopTypecombo->currentText();
    else if(dir == Right )index = ui->RightTypecombo->currentText();
    else if(dir == Left  )index = ui->LeftTypecombo->currentText();
    else if(dir == Bottom)index = ui->BottomTypecombo->currentText();
    else if(dir == Front )index = ui->FrontTypecombo->currentText();
    else if(dir == Back  )index = ui->BackTypecombo->currentText();
    return this->ConvertStringToBoundary(index);
}
QString CBoxDefineDialog::GetBoundaryName(BoundaryDir dir)const{
    if     (dir == Top   ){
        if(ui->TopNameEdit->text() == "")return "Top";
        else return ui->TopNameEdit   ->text();
    }else if(dir == Right ){
        if(ui->RightNameEdit->text() == "")return "Right";
        else return ui->RightNameEdit ->text();
    }else if(dir == Left  ){
        if(ui->LeftNameEdit->text() == "")return "Left";
        else return ui->LeftNameEdit  ->text();
    }else if(dir == Bottom){
        if(ui->BottomNameEdit->text() == "")return "Bottom";
        else return ui->BottomNameEdit->text();
    }else if(dir == Front ){
        if(ui->FrontNameEdit->text() == "")return "Front";
        else return ui->FrontNameEdit ->text();
    }else if(dir == Back  ){
        if(ui->BackNameEdit->text() == "")return "Back";
        else return ui->BackNameEdit  ->text();
    }else return "";
}

GradingType CBoxDefineDialog::GetGradigngType()const{
    GradingType type;
    if(ui->GradingCombo->currentText() == "simpleGrading")type = SimpleGrading;
    if(ui->GradingCombo->currentText() == "edgeGrading"  )type = EdgeGrading;
    return type;
}
void CBoxDefineDialog::SetGradigngType(GradingType type){
    if(type == SimpleGrading)ui->GradingCombo->setCurrentText("simpleGrading");
    if(type == EdgeGrading  )ui->GradingCombo->setCurrentText("edgeGrading");
}
void CBoxDefineDialog::SetBoundaryName(BoundaryDir dir,QString name){
    this->ConvertDirToNameEdit(dir)->setText(name);
}
void CBoxDefineDialog::SetBoundaryType(BoundaryDir dir,BoundaryType type){
    this->ConvertDirToCombo(dir)->setCurrentText(ConvertBoundaryToString(type));
}

void CBoxDefineDialog::ConnectionLock(BoundaryDir dir,bool lock){
    //ロック
    if(lock == true){
        this->ConvertDirToCombo(dir)->setCurrentText(ConvertBoundaryToString(BoundaryType::None));
        this->ConvertDirToCombo(dir)->setEnabled(true);
    }else{
        this->ConvertDirToCombo(dir)->setEnabled(false);
    }
}

//エラー判定
QString CBoxDefineDialog::FormatError()const{
    //引数の数があっているかな判定
    QString failed = "";

    //名前とタイプは全て一致
    QMap<QString,BoundaryType> map;
    for(int i=0;i<6;i++){
        BoundaryDir dir = static_cast<BoundaryDir>(i);

        //map内に同じ境界名が存在する。
        if(map.find(this->GetBoundaryName(dir)) != map.end()){
            if(map.find(this->GetBoundaryName(dir)).value() != this->GetBoundaryType(dir)){
                failed = "境界名と境界タイプが一致しない組み合わせがあります。";
            }
        }else{
            //追加
            map.insert(this->GetBoundaryName(dir),this->GetBoundaryType(dir));
        }
    }
    return failed;
}
//出力
void CBoxDefineDialog::ExportCBlock(){
    for(int i=0;i<6;i++){
        BoundaryDir dir = static_cast<BoundaryDir>(i);
        this->block->boundery[i] = this->GetBoundaryType(dir);
        this->block->name[i]     = this->GetBoundaryName(dir);
    }
    this->block->div[0] = this->ui->XspinBox->value();
    this->block->div[1] = this->ui->YspinBox->value();
    this->block->div[2] = this->ui->ZspinBox->value();
    this->block->grading   = this->GetGradigngType();
    for(int i=0;i<(this->GetGradigngType() == GradingType::SimpleGrading ? 3 : 12);i++){
        this->block->grading_args.push_back(this->grading_args[i]->value());
    }
    this->block->ReorderEdges();
    this->block->RefreshDividePoint();
}
void CBoxDefineDialog::ImportCBlock(){
    //面の設定
    for(int i = 0;i<6;i++){
        this->ConvertDirToNameEdit(static_cast<BoundaryDir>(i))->setText(this->block->name[i]);
        this->ConvertDirToCombo   (static_cast<BoundaryDir>(i))->setCurrentText(this->ConvertBoundaryToString(this->block->boundery[i]));
        types_log[i] = this->block->boundery[i];
    }
    this->ui->XspinBox->setValue(this->block->div[0]);
    this->ui->YspinBox->setValue(this->block->div[1]);
    this->ui->ZspinBox->setValue(this->block->div[2]);
    this->SetGradigngType(this->block->grading);
    for(int i=0;i<(this->GetGradigngType() == GradingType::SimpleGrading ? 3 : 12);i++){
        this->grading_args[i]->setValue(this->block->grading_args[i]);
    }
}

void CBoxDefineDialog::GradigngComboChanged(QString text){
    if(text == "simpleGrading"){
        this->SetGradigngType(SimpleGrading);
        for(int i=3;i<12;i++){
            this->grading_args[i]->hide();
        }
    }
    if(text == "edgeGrading"  ){
        this->SetGradigngType(EdgeGrading);
        for(int i=3;i<12;i++){
            this->grading_args[i]->show();
        }
    }
}
void CBoxDefineDialog::SyncOtherCombo(int){
    //全てのコネクトを解除
    for(int i=0;i<6;i++){
        disconnect(this->ConvertDirToCombo(static_cast<BoundaryDir>(i)),SIGNAL(currentIndexChanged(int)),this,SLOT(SyncOtherCombo(int)));
    }

    //すべての境界で
    for(int i=0;i<6;i++){
        if(types_log[i] != this->GetBoundaryType(static_cast<BoundaryDir>(i))){
            QString name = this->GetBoundaryName(static_cast<BoundaryDir>(i));

            //他の方向に対して
            for(int j=0;j<6;j++){
                if(i==j)continue;
                //名前が一致すれば
                if(name == this->GetBoundaryName(static_cast<BoundaryDir>(j))){
                    //コンボボックスを書き換える
                    this->SetBoundaryType(static_cast<BoundaryDir>(j),this->GetBoundaryType(static_cast<BoundaryDir>(i)));
                }
            }
        }
    }
    for(int i=0;i<6;i++){
        this->types_log[i] = this->GetBoundaryType(static_cast<BoundaryDir>(i));
    }
    //全てのコネクトを連結
    for(int i=0;i<6;i++){
        connect(this->ConvertDirToCombo(static_cast<BoundaryDir>(i)),SIGNAL(currentIndexChanged(int)),this,SLOT(SyncOtherCombo(int)));
    }
}

CBoxDefineDialog::CBoxDefineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CBoxDefineDialog)
{
    ui->setupUi(this);
    connect(this->ui->GradingCombo,SIGNAL(currentIndexChanged(QString)),this,SLOT(GradigngComboChanged(QString)));
    connect(this->ui->BackTypecombo  ,SIGNAL(currentIndexChanged(int)),this,SLOT(SyncOtherCombo(int)));
    connect(this->ui->FrontTypecombo ,SIGNAL(currentIndexChanged(int)),this,SLOT(SyncOtherCombo(int)));
    connect(this->ui->TopTypecombo   ,SIGNAL(currentIndexChanged(int)),this,SLOT(SyncOtherCombo(int)));
    connect(this->ui->RightTypecombo ,SIGNAL(currentIndexChanged(int)),this,SLOT(SyncOtherCombo(int)));
    connect(this->ui->LeftTypecombo  ,SIGNAL(currentIndexChanged(int)),this,SLOT(SyncOtherCombo(int)));
    connect(this->ui->BottomTypecombo,SIGNAL(currentIndexChanged(int)),this,SLOT(SyncOtherCombo(int)));

    for(int i=0;i<12;i++){
        QDoubleSpinBox* p = new QDoubleSpinBox();
        p->setValue(1.0);
        p->setMaximumSize(100,40);
        p->setDecimals(5);
        this->grading_args.push_back(p);
        this->ui->GradingArgsLayout->addWidget(this->grading_args[i],i/4,i%4);
        if(i >= 3)this->grading_args[i]->hide();
    }
    for(int i=0;i<6;i++){
        types_log[i] = static_cast<BoundaryType>(0);
    }
}

CBoxDefineDialog::~CBoxDefineDialog()
{
    delete ui;
}

void CBoxDefineDialog::AcceptProxy(){
    QString mes = this->FormatError();
    if(mes != ""){
        QMessageBox::information(this,"フォーマットエラー",mes,QMessageBox::Ok);
    }else{        
        //値の代入
        for(int i=0;i<6;i++){
            BoundaryDir dir = static_cast<BoundaryDir>(i);
            this->block->boundery[i] = this->GetBoundaryType(dir);
            this->block->name[i]     = this->GetBoundaryName(dir);
        }
        this->block->div[0] = this->ui->XspinBox->value();
        this->block->div[1] = this->ui->YspinBox->value();
        this->block->div[2] = this->ui->ZspinBox->value();
        this->block->grading   = this->GetGradigngType();
        this->block->grading_args.resize(this->GetGradigngType() == GradingType::SimpleGrading ? 3 : 12);
        for(int i=0;i<this->block->grading_args.size();i++){
            this->block->grading_args[i] = this->grading_args[i]->value();
        }
        this->block->RefreshDividePoint();

        accept();
    }
}
