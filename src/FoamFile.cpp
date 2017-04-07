#include "FoamFile.h"

void FoamFileValue::Inline(std::ostream& out, int depth)const{
    for(int i=0;i<depth;i++)out << TAB;
    out << this->name.toStdString().c_str() << " " << this->value.toStdString().c_str() << std::endl;
}
template<class T>
void FoamFileVector<T>::Inline(std::ostream& out, int depth)const{
    for(int i=0;i<depth;i++)out << TAB;
    out << "(";
    for(typename std::initializer_list<T>::iterator it = this->array.begin();it != array.end();it++){
        out << *it << " ";
    }
    out << "\b)" << std::endl;
}
template<class T>
void FoamFileDictionary<T>::Inline(std::ostream& out, int depth)const{
    for(int i=0;i<depth;i++)out << TAB;
    out << name.toStdString().c_str() << std::endl;
    for(int i=0;i<depth;i++)out << TAB;
    out << "(\n" << std::endl;
    for(typename std::initializer_list<T>::iterator it = this->array.begin();it != array.begin();it++){
        it->Inline(out,depth++);
    }
    for(int i=0;i<depth;i++)out << TAB;
    out << ");" << std::endl;
}
template<class T>
void FoamFileArray<T>::Inline(std::ostream& out, int depth)const{
    for(int i=0;i<depth;i++)out << TAB;
    out << name.toStdString().c_str() << std::endl;
    for(int i=0;i<depth;i++)out << TAB;
    out << "{\n" << std::endl;
    for(typename std::initializer_list<T>::iterator it = this->array.begin();it != array.begin();it++){
        it->Inline(out,depth++);
    }
    for(int i=0;i<depth;i++)out << TAB;
    out << "}" << std::endl;
}

//辞書追加
void FoamFile::AddToken(FoamFileToken* token){
     seqence.push_back(token);
}

//出力
void FoamFile::Export(QString filepath)const{
    std::ofstream out(filepath.toStdString().c_str());
    for(int i =0;i<this->seqence.size();i++){
        this->seqence[i]->Inline(out,0);
        out << std::endl;
    }
}

FoamFile::FoamFile()
{
}

FoamFile::~FoamFile()
{
}
