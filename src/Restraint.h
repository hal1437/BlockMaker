#ifndef RESTRAINT_H
#define RESTRAINT_H
#include <algorithm>
#include <vector>
#include "CObject/CObject.h"
#include "CObject/CLine.h"
#include "CObject/CArc.h"
#include "CObject/CSpline.h"

//拘束生成条件マクロ:全て同じ型
#define ALL_SAME_TYPE_RESTRAINTABLE(TYPE)                                                                           \
static bool Restraintable(const QVector<CObject *> nodes){                                                          \
    return (nodes.size()>=2 && std::all_of(nodes.begin(),nodes.end(),[](CObject* obj){return obj->is<TYPE>();}));   \
}
//監視コールバック:先頭に出す
#define SWAP_FRONT_CALLBACK                                 \
virtual void ChangeObjectCallback(CObject* obj){            \
    for(int i=1;i< this->nodes.size();i++){                 \
        if(this->nodes[i] == obj){                          \
            std::swap(this->nodes[0],this->nodes[i]);break; \
        }                                                   \
    }                                                       \
    this->Calc();                                           \
}
//監視コールバック:自信を削除
#define DESTROY_CALLBACK                        \
virtual void ChangeObjectCallback(CObject*){    \
    emit Destroy(this);                         \
}

//監視コールバック:自信を削除
#define DEFINE_ICON_PATH(PATH)      \
virtual QString GetIconPath()const{ \
    return QString(PATH);           \
}



enum RestraintType{
    EQUAL     ,//等しい値
    CONCURRENT,//平行
    EMPTY,
};

//幾何拘束
struct Restraint :public QObject{
    Q_OBJECT
protected:
    QVector<CObject*> nodes; //拘束対象
    RestraintType type;      //タイプ
    double value;            //拘束値

protected:
    //オブジェクトを監視対象にする
    void ObserveChild(CObject* obj);
    void IgnoreChild (CObject* obj);

public:
    CREATE_IO(double,Value,value)

    //型判別
    template<class T>
    bool is()const{
        return (dynamic_cast<const T*>(this) != nullptr);
    }

    //作成可能タイプを検索
    static QVector<RestraintType> Restraintable(const QVector<CObject*> nodes);

    //作成
    void Create(const QVector<CObject*> nodes,double value = 0);

    virtual void Calc() = 0;//拘束再計算
    virtual bool isComplete() = 0;//解決済みか判定

    virtual QString GetIconPath()const = 0;//アイコン
    virtual QVector<Pos> GetIconPoint()const;//アイコン表示点を取得


public slots:
    virtual void ChangeObjectCallback(CObject*){}

signals:
    void Changed(); //変更シグナル
    void Destroy(Restraint*); //自壊シグナル
};

//等しい値
class EqualLengthRestraint: public Restraint{
    Q_OBJECT
public:
    //CEdge限定
    ALL_SAME_TYPE_RESTRAINTABLE(CEdge)
    DEFINE_ICON_PATH(":/Restraint/EqualRestraint.png")
    virtual void Calc();
    virtual bool isComplete();

public slots:
    DESTROY_CALLBACK
};

//平行
class ConcurrentRestraint: public Restraint{
    Q_OBJECT
public:
    //CEdge限定
    ALL_SAME_TYPE_RESTRAINTABLE(CEdge)
    DEFINE_ICON_PATH(":/Restraint/ConcurrentRestraint.png")
    virtual void Calc();
    virtual bool isComplete();

public slots:
    DESTROY_CALLBACK
};


#endif // RESTRAINT_H
