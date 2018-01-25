#include "FoamFile.h"

//タブ出力
void FoamFile::TabOut(){
    for(int i=0;i<this->defs.size();i++)ofs << TAB;
}

void FoamFile::StartDictionaryDifinition(QString title){
    if(title != ""){
        this->TabOut();
        ofs << title << NEWLINE;
    }
    this->TabOut();
    ofs << "{"   << NEWLINE;
    this->defs.push_back(DEFINITON::DICTIONARY);
}
void FoamFile::StartListDifinition(QString title){
    if(title != ""){
        this->TabOut();
        ofs << title << NEWLINE;
    }
    this->TabOut();
    ofs << "("   << NEWLINE;
    this->defs.push_back(DEFINITON::LIST);
}


void FoamFile::OutVector(Pos pos){
    this->OutVector<double>({pos.x(),pos.y(),pos.z()});
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
    this->OutVectorInline<double>({pos.x(),pos.y(),pos.z()});
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
    if(defs[defs.size()-1] == DEFINITON::DICTIONARY) ofs << "}";
    if(defs[defs.size()-1] == DEFINITON::LIST      ) ofs << ")";
    if(defs.size() == 1)ofs  << ";";

    ofs << NEWLINE;
    defs.pop_back();
    if(defs.size()==0)ofs << NEWLINE;
}

void FoamFile::OutOpenFOAMHeader(){
    ofs << R"(
/*--------------------------------*- C++ -*----------------------------------*\
| =========                 |                                                 |
| \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox           |
|  \\    /   O peration     | Version:  4.1                                   |
|   \\  /    A nd           | Web:      www.OpenFOAM.org                      |
|    \\/     M anipulation  |                                                 |
\*---------------------------------------------------------------------------*/
)";
}
void FoamFile::OutBlockMakerHeader(){
    ofs <<R"(
/*---------------------------------------------------------------------------*\
|     ____  __           __      __  ___      __             |                |
|    / __ )/ /___  _____/ /__   /  |/  /___ _/ /_____  _____ | Block Maker    |
|   / __  / / __ \/ ___/ //_/  / /|_/ / __ `/ //_/ _ \/ ___/ | BlockMesh GUI  |
|  / /_/ / / /_/ / /__/ ,<    / /  / / /_/ / ,< /  __/ /     | Version: zero  |
| /_____/_/\____/\___/_/|_|  /_/  /_/\__,_/_/|_|\___/_/      |                |
\*---------------------------------------------------------------------------*/
)";

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
    this->ofs.setAutoDetectUnicode(true);
    //this->ofs.setCodec(QTextCodec::codecForName("UTF-8"));
    obj_name = str.split('/').back();
}
FoamFile::~FoamFile()
{
}
