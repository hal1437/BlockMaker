#ifndef CADMODELCORE_H
#define CADMODELCORE_H

#include <QObject>
#include <QStringList>
#include <iostream>
#include <fstream>
#include <SeqenceFileIO.h>
#include "CObject/CObject.h"
#include "CObject/CPoint.h"
#include "CObject/CEdge.h"
#include "CObject/CFace.h"
#include "CObject/CBlock.h"
#include "CObject/CStl.h"
#include "Dialog/SmartDimension.h"
#include "Restraint.h"

//監視変数追加
#define OBSERVER_BASE(TYPE,NAME)                  \
private:                                          \
    QList<TYPE> NAME;                             \
public slots:                                     \
    QList<TYPE>& Get##NAME()     {return NAME;}   \
    QList<TYPE>  Get##NAME()const{return NAME;}

//通常追加マクロ
#define OBSERVER_IO_COBJECT(TYPE,NAME)                                      \
inline void Add##NAME(TYPE value){                                          \
    connect(value,SIGNAL(Changed   ()                 ),this,SLOT(Update##NAME##Emittor()));    \
    connect(value,SIGNAL(Conflicted(CObject*,Conflict)),this,SLOT(ConflictAnyObjectEmittor(CObject*,Conflict)));    \
    connect(value,SIGNAL(Solved    (CObject*         )),this,SLOT(SolvedAnyObjectEmittor  (CObject*)));             \
    if(!exist(NAME,value)){                                                 \
        NAME.push_back(value);                                              \
        emit Update##NAME();                                                \
    }                                                                       \
}                                                                           \
inline void Remove##NAME(TYPE value){                                       \
    disconnect(value,SIGNAL(Changed   ()                 ),this,SLOT(Update##NAME##Emittor()));    \
    disconnect(value,SIGNAL(Conflicted(CObject*,Conflict)),this,SLOT(ConflictAnyObjectEmittor(CObject*,Conflict)));    \
    disconnect(value,SIGNAL(Solved    (CObject*         )),this,SLOT(SolvedAnyObjectEmittor  (CObject*)));             \
    NAME.removeAll(value);                                                  \
    emit Update##NAME();                                                    \
}

//自壊追加マクロ
#define OBSERVER_IO_RESTRAINT(TYPE,NAME)                                    \
inline void Add##NAME(TYPE value){                                          \
    connect(value,SIGNAL(Changed())    ,this,SLOT(Update##NAME##Emittor()));\
    connect(value,SIGNAL(Destroy(TYPE)),this,SLOT(Delete(TYPE)));           \
    connect(value,SIGNAL(Conflicted(CObject*,Conflict)),this,SLOT(ConflictAnyObjectEmittor(CObject*,Conflict)));    \
    connect(value,SIGNAL(Solved    (CObject*         )),this,SLOT(SolvedAnyObjectEmittor  (CObject*)));             \
    if(!exist(NAME,value)){                                                 \
        NAME.push_back(value);                                              \
        emit Update##NAME();                                                \
    }                                                                       \
}                                                                           \
inline void Remove##NAME(TYPE value){                                       \
    disconnect(value,SIGNAL(Changed()),this,SLOT(Update##NAME##Emittor())); \
    disconnect(value,SIGNAL(Destroy(TYPE)),this,SLOT(Delete(TYPE)));        \
    disconnect(value,SIGNAL(Conflicted(CObject*,Conflict)),this,SLOT(ConflictAnyObjectEmittor(CObject*,Conflict)));    \
    disconnect(value,SIGNAL(Solved    (CObject*         )),this,SLOT(SolvedAnyObjectEmittor  (CObject*)));             \
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

#define DEFINE_EMITTOR_CHECK_PAUSE(NAME) \
void NAME##Emittor(){                    \
    if(!this->isPause())emit NAME();     \
}


//クラス定義
class CadModelCore:public QObject
{
    Q_OBJECT

public:
    //オブジェクト定義
    DEFINE_OBSERVER_COBJECT(CObject*,Selected) //選択
    DEFINE_OBSERVER_COBJECT(CPoint* ,Points  ) //点
    DEFINE_OBSERVER_COBJECT(CEdge*  ,Edges   ) //線
    DEFINE_OBSERVER_COBJECT(CFace*  ,Faces   ) //面
    DEFINE_OBSERVER_COBJECT(CBlock* ,Blocks  ) //立体
    DEFINE_OBSERVER_COBJECT(CStl*   ,Stls    ) //STL

    DEFINE_OBSERVER_RESTRAINT(Restraint* ,Restraints)         //拘束

public:
    CPoint* origin; //原点
    bool pause = false;             //更新停止フラグ
    bool isPause()const{return this->pause;}

    //自動点結合
    void AutoMerge_impl(QList<CPoint*> points);
public:

    //選定して追加
    void AddObject(CObject* obj);

    //結合
    void Merge(QList<CPoint*> points); //先頭のものに結合
    void AutoMerge();                    //全てのオブジェクトの同一座標を自動的に結合
    void AutoMerge(CObject* obj);        //引数のオブジェクトの同一座標を自動的に結合

    //ファイル入出力
    bool ExportFoamFile(QString filename);
    bool ImportFoamFile(QString filename);

    //親取得
    QList<CBlock*> GetParent(CFace*  child)const;
    QList<CFace*>  GetParent(CEdge*  child)const;
    QList<CEdge*>  GetParent(CPoint* child)const;

    //選択解除
    void SelectedClear();

public slots:

    //エミッター
    DEFINE_EMITTOR_CHECK_PAUSE(UpdateSelected)
    DEFINE_EMITTOR_CHECK_PAUSE(UpdatePoints)
    DEFINE_EMITTOR_CHECK_PAUSE(UpdateEdges)
    DEFINE_EMITTOR_CHECK_PAUSE(UpdateFaces)
    DEFINE_EMITTOR_CHECK_PAUSE(UpdateBlocks)
    DEFINE_EMITTOR_CHECK_PAUSE(UpdateStls)
    DEFINE_EMITTOR_CHECK_PAUSE(UpdateRestraints)
    DEFINE_EMITTOR_CHECK_PAUSE(UpdateDimensions)
    DEFINE_EMITTOR_CHECK_PAUSE(UpdateAnyObject)
    DEFINE_EMITTOR_CHECK_PAUSE(UpdateAnyAction)

    void ConflictAnyObjectEmittor(CObject* object,Conflict conf);
    void SolvedAnyObjectEmittor  (CObject* object);

    //削除
    void Delete(CObject*   obj);
    void Delete(CPoint*    obj);
    void Delete(CEdge*     obj);
    void Delete(CFace*     obj);
    void Delete(CBlock*    obj);
    void Delete(CStl*      obj);
    void Delete(Restraint* obj);

    //更新操作関数
    void ObservePause();   //全ての子の監視を停止
    void ObserveRestart(); //全ての子の監視を再開

public:
    explicit CadModelCore(QWidget *parent = 0);
    ~CadModelCore();

signals:

    //更新シグナル
    void UpdateSelected   ();
    void UpdatePoints     ();
    void UpdateEdges      ();
    void UpdateFaces      ();
    void UpdateBlocks     ();
    void UpdateStls       ();
    void UpdateRestraints ();
    void UpdateDimensions ();
    void UpdateAnyObject  ();
    void UpdateAnyAction  ();

    //変更シグナル
    void ChangedPoints   (QList<CPoint*>);
    void ChangedEdges    (QList<CEdge* >);
    void ChangedFaces    (QList<CFace* >);
    void ChangedBlocks   (QList<CBlock*>);
    void ConflictAnyObject(CObject* object,Conflict conf);
    void SolvedAnyObject  (CObject* object);
};

class CadModelCoreInterface{
protected:
    CadModelCore* model = nullptr;
public:
    virtual void SetModel(CadModelCore* m){this->model = m;}
};


#endif // CADMODELCORE_H
