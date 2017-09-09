#ifndef RESTRAINT_H
#define RESTRAINT_H
#include <algorithm>
#include <vector>
#include "Point.h"
#include "CObject/CObject.h"
#include "CObject/CLine.h"
#include "CObject/CArc.h"
#include "CObject/CSpline.h"
#include "CObject/CSpline.h"

enum RestraintType{
    EQUAL      ,//等しい値
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
    void Create(const QVector<CObject*> nodes,double value);

    //拘束再計算
    virtual void Calc() = 0;


public slots:
    virtual void ChangeObjectCallback(CObject*){}

signals:
    void Changed();
};

//等しい値
class EqualLengthRestraint: public Restraint{
    Q_OBJECT
public:
    //CEdge限定
    static bool Restraintable(const QVector<CObject*> nodes);
    virtual void Calc();

public slots:
    virtual void ChangeObjectCallback(CObject*);
};

#endif // RESTRAINT_H
