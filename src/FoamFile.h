#ifndef FOAMFILE_H
#define FOAMFILE_H
#include <QVector>
#include <QDebug>
#include <QMap>
#include <fstream>
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

    QVector<DEFINITON> defs;
    std::ofstream ofs;

    //タブ出力
    void TabOut();
public:

    //定義開始
    void StartDictionaryDifinition(QString title); //辞書{}
    void StartListDifinition(QString title);       //リスト()
    //各出力
    template<class T>
    void OutVector(QVector<T> vector){
        this->TabOut();
        ofs << VectorToString(vector).toStdString() << std::endl;
    }
    void OutValue(QString name,QString value);     //値出力
    void OutString(QString str);    //文字列出力
    void EndScope();//閉じかっこ出力

    //ベクトルを文字列に変換
    template<class T>
    QString VectorToString(QVector<T> vector){
        QString ans;
        ans += "(";
        for(int i=0;i<vector.size();i++){
            ans += QString::number(vector[i]);
            if(i==vector.size() - 1)ans += ")";
            else                    ans += " ";
        }
        return ans;
    }
    //ファイル操作
    void Open(QString filepath);
    void Close(QString filepath);

    FoamFile();
    ~FoamFile();
};

#endif // FOAMFILE_H
