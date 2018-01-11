#include "SeqenceFileIO.h"

void SeqenceFileIO::OutputQString(const QString& str){
    this->stream << str.size();
    for(int i=0;i<str.size();i++){
        QChar cc = str[i];
        this->stream << cc;
    }
}
void SeqenceFileIO::InputQString (QString& str){
    int size;
    this->stream >> size;
    for(int i=0;i<size;i++){
        QChar cc;
        this->stream >> cc;
        str.push_back(cc);
    }
}


SeqenceFileIO::SeqenceFileIO(QString str):
    file(str)
{
    if (!file.open(QIODevice::ReadWrite)){
        QMessageBox::information(nullptr, "ファイルが開けません",file.errorString());
    }else{
        this->stream.setDevice(&file);
    }
}
