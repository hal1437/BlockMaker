#include "FoamFile.h"
//タブ出力
void FoamFile::TabOut(){
    for(int i=0;i<this->defs.size();i++)ofs << TAB;
}

void FoamFile::StartDictionaryDifinition(QString title){
    ofs << title.toStdString().c_str() << std::endl;
    ofs << "{"                         << std::endl;
    this->defs.push_back(DEFINITON::DICTIONARY);
}
void FoamFile::StartListDifinition(QString title){
    ofs << title.toStdString().c_str() << std::endl;
    ofs << "("                         << std::endl;
    this->defs.push_back(DEFINITON::LIST);
}


void FoamFile::OutValue(QString name,QString value){
    this->TabOut();
    ofs << name.toStdString().c_str() << " " << value.toStdString().c_str() << std::endl;
}
void FoamFile::OutString(QString str){
    ofs << str.toStdString().c_str() << std::endl;
}
void FoamFile::EndScope(){
    if(defs.empty()){
        qDebug() << "Error. FoamFile used EndScope() when defs.empty()";
        return;
    }
    for(int i=0;i<this->defs.size() - 1;i++) ofs << TAB;
    if(defs[defs.size()-1] == DEFINITON::DICTIONARY) ofs << "}" << std::endl;
    if(defs[defs.size()-1] == DEFINITON::LIST      ) ofs << ")"  << std::endl;
}


void FoamFile::Open(QString filepath){
    ofs = std::ofstream(filepath.toStdString().c_str());
}

void FoamFile::Close(QString filepath){
    ofs.close();
}


FoamFile::FoamFile()
{
}

FoamFile::~FoamFile()
{
}
