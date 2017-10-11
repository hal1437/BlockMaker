#ifndef RESTRAINT_H
#define RESTRAINT_H
#include <algorithm>
#include <vector>
#include "CObject/CObject.h"
#include "CObject/CLine.h"
#include "CObject/CArc.h"
#include "CObject/CSpline.h"
#include "CObject/CFileEdge.h"

//拘束生成条件マクロ:全て同じ型
#define ALL_SAME_TYPE_RESTRAINTABLE(TYPE,MIN_COUNT)                                                                         \
static bool Restraintable(const QVector<CObject *> nodes){                                                                  \
    return (nodes.size()>=MIN_COUNT && std::all_of(nodes.begin(),nodes.end(),[](CObject* obj){return obj->is<TYPE>();}));   \
}
//拘束生成条件マクロ:1対n
#define ONCE_AND_ANY_TYPE_RESTRAINTABLE(TYPE1,TYPE2,MIN_TYPE2_COUNT)                                                     \
static bool Restraintable(const QVector<CObject *> nodes){                                                               \
    return (std::any_of  (nodes.begin(),nodes.end(),[](CObject* obj){return (obj->is<TYPE1>() && !obj->is<TYPE2>());}) && \
            std::count_if(nodes.begin(),nodes.end(),[](CObject* obj){return (obj->is<TYPE2>());}) >= MIN_TYPE2_COUNT);   \
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
    if(!this->isComplete())emit Destroy(this);  \
}

//メンバ関数簡易定義
#define DEFINE_ICON_PATH(PATH) virtual QString GetIconPath()const{ return QString(PATH);}
#define DEFINE_RESTRAINT_NAME(NAME) virtual QString GetRestraintName()const{ return QString(NAME);}


enum RestraintType{
    EQUAL      , //等しい値
    CONCURRENT , //平行
    LOCK       , //固定
    UNLOCK     , //固定解除
    MATCH      , //一致
    EMPTY      , //無効な拘束
};

//幾何拘束
struct Restraint :public QObject{
    Q_OBJECT
protected:
    QVector<CObject*> nodes; //拘束対象
    RestraintType type;      //タイプ

protected:
    //オブジェクトを監視対象にする
    void ObserveChild(CObject* obj);
    void IgnoreChild (CObject* obj);

public:

    //型判別
    template<class T>
    bool is()const{
        return (dynamic_cast<const T*>(this) != nullptr);
    }

    //作成可能タイプを検索
    static QVector<Restraint*> Restraintable(const QVector<CObject*> nodes);

    //作成
    void Create(const QVector<CObject*> nodes);

    virtual void Calc() = 0;//拘束再計算
    virtual bool isComplete(){return true;}//解決済みか判定

    virtual QString GetIconPath      ()const = 0;//アイコンパス
    virtual QString GetRestraintName ()const = 0;//拘束名
    virtual QVector<Pos> GetIconPoint()const;//アイコン表示点を取得

    Restraint(QVector<CObject*> nodes = QVector<CObject*>());

public slots:
    DESTROY_CALLBACK //変更時に自壊する

signals:
    void Changed(); //変更シグナル
    void Destroy(Restraint*); //自壊シグナル
};

//等しい値
class EqualLengthRestraint: public Restraint{
    Q_OBJECT
private:
    double length=0;//長さ
public:
    //CEdge限定
    ALL_SAME_TYPE_RESTRAINTABLE(CEdge,2)
    DEFINE_ICON_PATH(":/Restraint/EqualRestraint.png")
    DEFINE_RESTRAINT_NAME("等値")

    virtual void Create(const QVector<CObject*> nodes);
    virtual void Calc();
    virtual bool isComplete();
    EqualLengthRestraint(QVector<CObject*> nodes = QVector<CObject*>()):Restraint(nodes){}

};

//平行
class ConcurrentRestraint: public Restraint{
    Q_OBJECT
public:
    //CLine限定
    ALL_SAME_TYPE_RESTRAINTABLE(CLine,2)
    DEFINE_ICON_PATH(":/Restraint/ConcurrentRestraint.png")
    DEFINE_RESTRAINT_NAME("平行")

    virtual void Calc();
    virtual bool isComplete();
    ConcurrentRestraint(QVector<CObject*> nodes = QVector<CObject*>()):Restraint(nodes){}
};

//固定
class LockRestraint: public Restraint{
    Q_OBJECT
public:
    //ロックされていないオブジェクト以上であれば全て可
    static bool Restraintable(const QVector<CObject *> nodes);
    DEFINE_ICON_PATH(":/Restraint/LockRestraint.png")
    DEFINE_RESTRAINT_NAME("固定")
    virtual void Calc();
    virtual bool isComplete();
    LockRestraint(QVector<CObject*> nodes = QVector<CObject*>()):Restraint(nodes){}
public slots:
    //自壊コールバックのオーバーライド
    virtual void ChangeObjectCallback(CObject*);
};
//固定解除
class UnlockRestraint: public Restraint{
    Q_OBJECT
public:
    //ロックされているオブジェクト以上であれば全て可
    static bool Restraintable(const QVector<CObject *> nodes);
    DEFINE_ICON_PATH(":/Restraint/UnlockRestraint.png")
    DEFINE_RESTRAINT_NAME("固定解除")
    virtual void Calc();
    UnlockRestraint(QVector<CObject*> nodes = QVector<CObject*>()):Restraint(nodes){}
};

//一致
class MatchRestraint: public Restraint{
    Q_OBJECT
public:
    //全てのオブジェクトに対してCPointは一致可能
    ONCE_AND_ANY_TYPE_RESTRAINTABLE(CObject,CPoint,1)
    DEFINE_ICON_PATH(":/Restraint/MatchRestraint.png")
    virtual QVector<Pos> GetIconPoint()const;//アイコン表示点を取得
    DEFINE_RESTRAINT_NAME("一致")
    virtual void Calc();
    virtual bool isComplete();
    MatchRestraint(QVector<CObject*> nodes = QVector<CObject*>()):Restraint(nodes){}
};


#endif // RESTRAINT_H
