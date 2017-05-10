#include "FoamFile.h"

//タブ出力
void FoamFile::TabOut(){
    for(int i=0;i<this->defs.size();i++)ofs << TAB;
}

void FoamFile::StartDictionaryDifinition(QString title){
    this->TabOut();
    ofs << title << NEWLINE;
    this->TabOut();
    ofs << "{"   << NEWLINE;
    this->defs.push_back(DEFINITON::DICTIONARY);
}
void FoamFile::StartListDifinition(QString title){
    this->TabOut();
    ofs << title << NEWLINE;
    this->TabOut();
    ofs << "("   << NEWLINE;
    this->defs.push_back(DEFINITON::LIST);
}


void FoamFile::OutVector(Pos pos){
    this->OutVector<double>({pos.x,pos.y,pos.z});
}
void FoamFile::OutValue(QString name,QString value){
    this->TabOut();
    ofs << name << " " << value << ";" << NEWLINE;
    if(defs.size()==0)ofs << NEWLINE;
}
void FoamFile::OutString(QString str){
    this->TabOut();
    ofs << str << NEWLINE;
}
void FoamFile::OutVectorInline(Pos pos){
    this->OutVectorInline<double>({pos.x,pos.y,pos.z});
}
void FoamFile::OutValueInline(QString name,QString value){
    ofs << name << " " << value << " ";
}
void FoamFile::OutStringInline(QString str){
    ofs << str << " ";
}
void FoamFile::EndScope(){
    if(defs.empty()){
        qDebug() << "Error. FoamFile used EndScope() when defs.empty()";
        return;
    }
    for(int i=0;i<this->defs.size() - 1;i++) ofs << TAB;
    if(defs[defs.size()-1] == DEFINITON::DICTIONARY) ofs << "}"  << NEWLINE;
    if(defs[defs.size()-1] == DEFINITON::LIST      ) ofs << ");"  << NEWLINE;
    defs.pop_back();
    if(defs.size()==0)ofs << NEWLINE;
}
void FoamFile::OutHeader(){
    this->StartDictionaryDifinition("FoamFile");
    this->OutValue("version" ,"2.0");
    this->OutValue("format " ,"ascii");
    this->OutValue("class  " ,"dictionary");
    this->OutValue("object " ,obj_name);

    this->EndScope();
}
void FoamFile::OutNewline(){
    ofs << NEWLINE;
}

FoamFile::FoamFile(QString str):
    file(str),ofs(&file)
{
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(nullptr, "ファイルが開けません",file.errorString());
        return;
    }
    obj_name = str.split('/').back();
}
FoamFile::~FoamFile()
{
}
