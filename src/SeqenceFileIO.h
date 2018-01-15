#ifndef SEQENCEFILEIO_H
#define SEQENCEFILEIO_H
#include <QFile>
#include <QDataStream>
#include <QMessageBox>

//順列ファイル出力
class SeqenceFileIO
{
public:
    QFile file;
    QDataStream stream;

public:
    void OutputQString(const QString& str);
    void InputQString (      QString& str);
public:

    template<class T> SeqenceFileIO& Output(const T& value){
        this->stream << value;
        return (*this);
    }
    template<class T> SeqenceFileIO& Output(const QList<T>& value){
        this->stream << value.size();
        for(T& t : value){
            this->stream << t;
        }
        return (*this);
    }
    template<class T> SeqenceFileIO& Input(T& value){
        this->stream >> value;
        return (*this);
    }
    template<class T> SeqenceFileIO& Input(QList<T>& value){
        int size;
        this->stream >> size;
        for(int i =0;i<size;i++){
            T t;
            this->stream >> t;
            value.push_back(t);
        }
        return *this;
    }
    //Function: void ee(T);
    template<class T,class Lambda> SeqenceFileIO& OutputForeach(const QList<T>& value,Lambda ee){
        this->stream << value.size();
        for(const T& t : value){
            ee(t);
        }
        return (*this);
    }
    //Function: T ee(SeqenceFileIO&);
    template<class T,class Lambda> SeqenceFileIO& InputForeach (QList<T>& value,Lambda ee){
        int size;
        this->stream >> size;
         for(int i =0;i<size;i++){
            value.push_back(ee(*this));
        }
        return  *this;
    }
    template<class T> SeqenceFileIO& operator<<(const T& value){
        return this->Output(value);
    }
    template<class T> SeqenceFileIO& operator<<(const QList<T>& value){
        return this->Output(value);
    }
    template<class T> SeqenceFileIO& operator>>(T& value){
        return this->Input(value);
    }
    template<class T> SeqenceFileIO& operator>>(QList<T>& value){
        return this->Input(value);
    }

    SeqenceFileIO(QString str);
};

#endif // SEQENCEFILEIO_H
