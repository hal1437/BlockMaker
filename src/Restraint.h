#ifndef RESTRAINT_H
#define RESTRAINT_H
#include <algorithm>
#include <vector>
#include <QGLWidget>
#include "CObject/CObject.h"
#include "CObject/CLine.h"
#include "CObject/CArc.h"
#include "CObject/CSpline.h"
#include "CObject/CFileEdge.h"

//メンバ関数簡易定義
#define DEFINE_ICON_PATH(PATH)      virtual QString GetIconPath()     const{ return QString(PATH);}
#define DEFINE_RESTRAINT_NAME(NAME) virtual QString GetRestraintName()const{ return QString(NAME);}
#define DEFINE_SAME_CLONE(TYPE)     virtual CObject* TYPE::Clone()const{    \
    TYPE* c = new TYPE();                                                   \
    c->children    = this->children;                                        \
    c->stack_level = this->stack_level;                                     \
    return c;                                                               \
}


enum RestraintType{
    EQUAL      , //等しい値
    CONCURRENT , //平行
    VERTICAL   , //垂直
    LOCK       , //固定
    UNLOCK     , //固定解除
    MATCH      , //一致
    EMPTY      , //無効な拘束
};

//幾何拘束
struct Restraint :public CObject{
    Q_OBJECT
protected:
    QList<CObject*> children; //拘束対象
    RestraintType type;         //タイプ
    DEFINE_FLAG(NotEffect,false)
    OBSERVE_MEMBER(int  ,StackLevel,stack_level) //同一の座標に表示時に右に避ける

protected:
    //拘束生成条件マクロ:全て同じ型
    template<class Type,int MIN_COUNT>
    static bool AllSameTypeRestraint(const QList<CObject *> nodes){
        return (nodes.size() >= MIN_COUNT &&
                std::all_of(nodes.begin(),nodes.end(),[](CObject* obj){return obj->is<Type>();}));
    }
    //拘束生成条件マクロ:1対n
    template<class Type1,class Type2,int MIN_TYPE2_COUNT>
    static bool OnceAndAnyTypeRestraintable(const QList<CObject *> nodes){
        return (std::any_of  (nodes.begin(),nodes.end(),[](CObject* obj){return (obj->is<Type1>() && !obj->is<Type2>());}) &&
                std::count_if(nodes.begin(),nodes.end(),[](CObject* obj){return (obj->is<Type2>());}) >= MIN_TYPE2_COUNT);
    }
    //監視コールバック:先頭に出す
    virtual void SwapFrontCallback(CObject* obj){
        for(int i=1;i< this->children.size();i++){
            if(this->children[i] == obj){
                std::swap(this->children[0],this->children[i]);break;
            }
        }
        this->Calc();
    }
    //監視コールバック:不成立時に自身を削除
    virtual void DestroyCallback(){
        if(!this->isComplete())emit Destroy(this);
    }

public:
    virtual QString DefaultClassName(){return this->GetRestraintName();}

    virtual void DrawGL(Pos camera,Pos center)const;

    //作成可能タイプを検索
    static QList<Restraint*> Restraintable(const QList<CObject*> children);

    //子の操作
    virtual CObject* GetChild     (int index);
    virtual void     SetChild     (int index,CObject* obj);
    virtual int      GetChildCount()const;
    virtual CObject* Clone()const;

    //作成
    void Create(const QList<CObject*> nodes);

    virtual void Calc(){} //拘束再計算
    virtual bool isComplete(){return true;}//解決済みか判定

    virtual QString GetIconPath      ()const = 0; //アイコンパス
    virtual QString GetRestraintName ()const = 0; //拘束名
    virtual QList<Pos> GetIconPoint()const;     //アイコン表示点を取得

    virtual Pos GetNearPos (const Pos&)const{return Pos();}
    virtual Pos GetNearLine(const Pos&,const Pos&)const{return Pos();}

    Restraint(QList<CObject*> nodes = QList<CObject*>());

public slots:

    //子変更コールバック
    virtual void ChangeChildCallback(QList<CObject*>){
        if(!this->isComplete()){
            //監視コールバック:不成立時に自身を削除
            //DestroyCallback();
        }
    }

signals:
    void Destroy(Restraint*); //自壊シグナル
};

//等しい値
class EqualLengthRestraint: public Restraint{
    Q_OBJECT
private:
    double length = 0;//長さ
public:
    //CEdge限定
    static  bool Restraintable(const QList<CObject*> vs){return AllSameTypeRestraint<CEdge,2>(vs);}
    virtual QString GetIconPath()     const{ return QString(":/Restraint/EqualRestraint.png");}
    virtual QString GetRestraintName()const{ return QString("等値");}

    virtual void Create(const QList<CObject*> nodes);
    virtual void Calc();
    virtual bool isComplete();
    EqualLengthRestraint(QList<CObject*> nodes = QList<CObject*>()):Restraint(nodes){}

};

//平行
class ConcurrentRestraint: public Restraint{
    Q_OBJECT
public:
    //CLine限定
    static  bool Restraintable(const QList<CObject*> vs){return AllSameTypeRestraint<CLine,2>(vs);}
    virtual QString GetIconPath()     const{ return QString(":/Restraint/ConcurrentRestraint.png");}
    virtual QString GetRestraintName()const{ return QString("平行");}

    virtual void Calc();
    virtual bool isComplete();
    ConcurrentRestraint(QList<CObject*> nodes = QList<CObject*>()):Restraint(nodes){}
};
//垂直
class VerticalRestraint: public Restraint{
    Q_OBJECT
public:
    //CEdgeを2つ以上含む、かつ他のCEdgeの端点が1つめのCEdgeに一致している
    static bool Restraintable(const QList<CObject*> vs);
    virtual QString GetIconPath()     const{ return QString(":/Restraint/CrossRestraint.png");}
    virtual QString GetRestraintName()const{ return QString("垂直");}

    virtual void Calc();
    virtual bool isComplete();
    VerticalRestraint(QList<CObject*> nodes = QList<CObject*>()):Restraint(nodes){}
};


//固定
class LockRestraint: public Restraint{
    Q_OBJECT
public:
    //ロックされていないオブジェクト以上であれば全て可

    static bool Restraintable(const QList<CObject *> nodes);
    virtual QString GetIconPath()     const{ return QString(":/Restraint/LockRestraint.png");}
    virtual QString GetRestraintName()const{ return QString("固定");}
    virtual void Calc();
    virtual bool isComplete();
    LockRestraint(QList<CObject*> nodes = QList<CObject*>()):Restraint(nodes){}
public slots:
    //自壊コールバックのオーバーライド
    virtual void ChangeObjectCallback(CObject*);
};
//固定解除
class UnlockRestraint: public Restraint{
    Q_OBJECT
public:
    //ロックされているオブジェクト以上であれば全て可
    static bool Restraintable(const QList<CObject *> nodes);
    virtual QString GetIconPath()     const{ return QString(":/Restraint/UnlockRestraint.png");}
    virtual QString GetRestraintName()const{ return QString("固定解除");}

    virtual void Calc();
    UnlockRestraint(QList<CObject*> nodes = QList<CObject*>()):Restraint(nodes){}
};

//一致
class MatchRestraint: public Restraint{
    Q_OBJECT
public:
    //全てのオブジェクトに対してCPointは一致可能
    static bool Restraintable(const QList<CObject *> nodes){return OnceAndAnyTypeRestraintable<CObject,CPoint,1>(nodes);}
    virtual QString GetIconPath()     const{ return QString(":/Restraint/MatchRestraint.png");}
    virtual QString GetRestraintName()const{ return QString("一致");}
    virtual QList<Pos> GetIconPoint()const; //特別定義

    virtual void Calc();
    virtual bool isComplete();
    MatchRestraint(QList<CObject*> nodes = QList<CObject*>()):Restraint(nodes){}
};


#endif // RESTRAINT_H
