#ifndef FOAMFILE_H
#define FOAMFILE_H
#include <QVector>
#include <QDebug>
#include <QMap>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include "Utils.h"


//FOAMファイル
class FoamFile
{

private:

    //括弧タイプ
    enum DEFINITON{
        DICTIONARY,
        LIST ,
    };

    QVector<DEFINITON> defs; //スコープ深さ
    QFile file;              //ファイル
    QTextStream ofs;         //出力ストリーム
    QString obj_name;        //オブジェクトネーム

public:
    //ベクトルを文字列に変換
    template<class T>
    static QString VectorToString(QVector<T> vector);

public:

    //タブ出力
    void TabOut();

    //定義開始
    void StartDictionaryDifinition(QString title); //辞書{}
    void StartListDifinition(QString title);       //リスト()

    //各出力
    template<class T>
    void OutVector(QVector<T> vector);         //ベクトル出力
    void OutValue(QString name,QString value); //値出力
    void OutString(QString str);               //文字列出力
    void EndScope();                           //閉じかっこ出力
    void OutHeader();                          //ヘッダー出力

    //改行&タブなし出力
    template<class T>
    void OutVectorInline(QVector<T> vector);         //ベクトル出力
    void OutValueInline(QString name,QString value); //値出力
    void OutStringInline(QString str);               //文字列出力



    FoamFile(QString str);
    ~FoamFile();
};

template<class T>
void FoamFile::OutVector(QVector<T> vector){
    this->TabOut();
    ofs << VectorToString(vector) << NEWLINE;
}
template<class T>
void FoamFile::OutVectorInline(QVector<T> vector){
    ofs << VectorToString(vector) << " ";
}

template<class T>
QString FoamFile::VectorToString(QVector<T> vector){
    QString ans;
    ans += "(";
    for(int i=0;i<vector.size();i++){
        ans += QString::number(vector[i]);
        if(i==vector.size() - 1)ans += ")";
        else                    ans += " ";
    }
    return ans;
}


#endif // FOAMFILE_H
