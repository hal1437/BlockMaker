#ifndef CADMODELCORE_H
#define CADMODELCORE_H

#include <QObject>
#include <QStringList>
#include <iostream>
#include <fstream>
#include "CObject/CObject.h"
#include "CObject/CPoint.h"
#include "CObject/CEdge.h"
#include "CObject/CFace.h"
#include "CObject/CBlock.h"
#include "CObject/CStl.h"
#include "Dialog/SmartDimension.h"
#include "Restraint.h"


//監視変数追加
#define OBSERVER_BASE(TYPE,NAME)                    \
private:                                            \
    QVector<TYPE> NAME;                             \
public slots:                                       \
    QVector<TYPE>& Get##NAME()     {return NAME;}   \
    QVector<TYPE>  Get##NAME()const{return NAME;}

//通常追加マクロ
#define OBSERVER_IO_COBJECT(TYPE,NAME)                                      \
inline void Add##NAME(TYPE value){                                          \
    connect(value,SIGNAL(Changed()),this,SLOT(Update##NAME##Emittor()));    \
    if(!exist(NAME,value)){                                                 \
        NAME.push_back(value);                                              \
        emit Update##NAME();                                                \
    }                                                                       \
}                                                                           \
inline void Remove##NAME(TYPE value){                                       \
    disconnect(value,SIGNAL(Changed()),this,SLOT(Update##NAME##Emittor())); \
    NAME.removeAll(value);                                                  \
    emit Update##NAME();                                                    \
}

//自壊追加マクロ
#define OBSERVER_IO_RESTRAINT(TYPE,NAME)                                    \
inline void Add##NAME(TYPE value){                                          \
    connect(value,SIGNAL(Changed())    ,this,SLOT(Update##NAME##Emittor()));\
    connect(value,SIGNAL(Destroy(TYPE)),this,SLOT(Delete(TYPE)));           \
    if(!exist(NAME,value)){                                                 \
        NAME.push_back(value);                                              \
        emit Update##NAME();                                                \
    }                                                                       \
}                                                                           \
inline void Remove##NAME(TYPE value){                                       \
    disconnect(value,SIGNAL(Changed()),this,SLOT(Update##NAME##Emittor())); \
    disconnect(value,SIGNAL(Destroy(TYPE)),this,SLOT(Delete(TYPE)));        \
    NAME.removeAll(value);                                                  \
    emit Update##NAME();                                                    \
}

//監視オブジェクトの定義、関数宣言を行う
#define DEFINE_OBSERVER_COBJECT(TYPE,NAME)          \
    OBSERVER_BASE(TYPE,NAME)                        \
    OBSERVER_IO_COBJECT(TYPE,NAME)
#define DEFINE_OBSERVER_RESTRAINT(TYPE,NAME)        \
    OBSERVER_BASE(TYPE,NAME)                        \
    OBSERVER_IO_RESTRAINT(TYPE,NAME)


//クラス定義
class CadModelCore:public QObject
{
    Q_OBJECT

public:
    //オブジェクト定義
    DEFINE_OBSERVER_COBJECT(CObject*  ,Selected  )
    DEFINE_OBSERVER_COBJECT(CPoint*   ,Points    )
    DEFINE_OBSERVER_COBJECT(CEdge*    ,Edges     )
    DEFINE_OBSERVER_COBJECT(CFace*    ,Faces     )
    DEFINE_OBSERVER_COBJECT(CBlock*   ,Blocks    )
    DEFINE_OBSERVER_COBJECT(CStl*     ,Stls      )

    DEFINE_OBSERVER_RESTRAINT(Restraint*      ,Restraints)
    //DEFINE_OBSERVER(SmartDimension* ,Dimensions)

public:
    CPoint* origin; //原点
    DEFINE_FLAG(Pause,false)             //更新停止フラグ

    //自動点結合
    void AutoMerge_impl(QVector<CPoint*> points);
public:

    //選定して追加
    void AddObject(CObject* obj);

    //結合
    void Merge(QVector<CPoint*> points); //先頭のものに結合
    void AutoMerge();                    //全てのオブジェクトの同一座標を自動的に結合
    void AutoMerge(CObject* obj);        //引数のオブジェクトの同一座標を自動的に結合

    //ファイル入出力
    bool ExportFoamFile(QString filename)const;
    bool ImportFoamFile(QString filename);

    //親取得
    QVector<CBlock*> GetParent(CFace*  child)const;
    QVector<CFace*>  GetParent(CEdge*  child)const;
    QVector<CEdge*>  GetParent(CPoint* child)const;

    //選択解除
    void SelectedClear();

public slots:

    //Emittor
    DEFINE_EMITTOR(UpdateSelected)
    DEFINE_EMITTOR(UpdatePoints)
    DEFINE_EMITTOR(UpdateEdges)
    DEFINE_EMITTOR(UpdateFaces)
    DEFINE_EMITTOR(UpdateBlocks)
    DEFINE_EMITTOR(UpdateStls)
    DEFINE_EMITTOR(UpdateRestraints)
    DEFINE_EMITTOR(UpdateDimensions)
    DEFINE_EMITTOR(UpdateAnyObject)
    DEFINE_EMITTOR(UpdateAnyAction)

    //削除
    void Delete(CObject*   obj);
    void Delete(CPoint*    obj);
    void Delete(CEdge*     obj);
    void Delete(CFace*     obj);
    void Delete(CBlock*    obj);
    void Delete(CStl*      obj);
    void Delete(Restraint* obj);

    //更新操作関数
    void UpdatePause();   //停止
    void UpdateRestart(); //再始動

public:
    explicit CadModelCore(QWidget *parent = 0);
    ~CadModelCore();

signals:
    void SelectObjectChanged();
    void UpdateSelected  ();
    void UpdatePoints    ();
    void UpdateEdges     ();
    void UpdateFaces     ();
    void UpdateBlocks    ();
    void UpdateStls      ();
    void UpdateRestraints();
    void UpdateDimensions();
    void UpdateAnyObject ();
    void UpdateAnyAction ();
    void ChangedPoints   (QVector<CPoint*>);
    void ChangedEdges    (QVector<CEdge* >);
    void ChangedFaces    (QVector<CFace* >);
    void ChangedBlocks   (QVector<CBlock*>);
};

class CadModelCoreInterface{
protected:
    CadModelCore* model = nullptr;
public:
    virtual void SetModel(CadModelCore* m){this->model = m;}
};


#endif // CADMODELCORE_H
