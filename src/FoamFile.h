#ifndef FOAMFILE_H
#define FOAMFILE_H
#include <QVector>
#include <QMap>
#include <fstream>
#include <initializer_list>
#include "Utils.h"


struct FoamFileToken{
    virtual void Inline(std::ostream& out, int depth)const = 0;
};

//値
struct FoamFileValue : public FoamFileToken{
    QString name;
    QString value;
    virtual void Inline(std::ostream& out, int depth)const = 0;
    FoamFileValue(QString name,QString value) : name(name),value(value){};
};

//ベクトル
template<class T>
struct FoamFileVector : public FoamFileToken{
    QString name;
    std::initializer_list<T> array;
    void Inline(std::ostream& out, int depth)const;
    FoamFileVector(QString name,std::initializer_list<T> array) : name(name),array(array){};
};

//辞書
template<class T>
struct FoamFileDictionary : public FoamFileToken{
    QString name;
    std::initializer_list<T> array;
    virtual void Inline(std::ostream& out, int depth)const = 0;
    FoamFileDictionary(QString name,std::initializer_list<T> array) : name(name),array(array){};
};

//配列
template<class T>
struct FoamFileArray : public FoamFileToken{
    QString name;
    std::initializer_list<T> array;
    virtual void Inline(std::ostream& out, int depth)const = 0;
    FoamFileArray(QString name,std::initializer_list<T> array) : name(name),array(array){};
};


class FoamFile
{
private:
    QVector<FoamFileToken*> seqence;

public:

    //辞書追加
    void AddToken(FoamFileToken* token);

    //出力
    void Export(QString filepath)const;

    FoamFile();
    ~FoamFile();
};

#endif // FOAMFILE_H
