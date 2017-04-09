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

template<T>
void FoamFile::OutVector(QVector<T> vector){
    this->TabOut();
    ofs << VectorToString(vector) << std::endl;
}
void FoamFile::OutValue(QString name,QString value){
    this->TabOut();
    ofs << name << " " << value << std::endl;
}

void FoamFile::OutString(QString str){
    ofs << str << std::endl;
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


template<T>
void FoamFile::VectorToString(QVector<T> vector){
    QString ans;
    ans += "(";
    for(int i=0;i<vector.size();i++){
        ans += QString::number(vector[i]);
        if(i==vector.size() - 1)ans += ")";
        else                    ans += " ";
    }
    return ans;
}

void Open(QString filepath);
void Close(QString filepath);


FoamFile::FoamFile()
{
}

FoamFile::~FoamFile()
{
}
