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
#include "Dialog/SmartDimension.h"
#include "Restraint.h"



class CadModelCore:public QObject
{
    Q_OBJECT

public:
    //オブジェクト定義
    DEFINE_OBSERVER(CObject*        ,Selected  )
    DEFINE_OBSERVER(CPoint*         ,Points    )
    DEFINE_OBSERVER(CEdge*          ,Edges     )
    DEFINE_OBSERVER(CFace*          ,Faces     )
    DEFINE_OBSERVER(CBlock*         ,Blocks    )
    DEFINE_OBSERVER(Restraint*      ,Restraints)
    //DEFINE_OBSERVER(SmartDimension* ,Dimensions)

public:
    CPoint* origin; //原点

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

    //削除
    void Delete(CBlock* obj);
    void Delete(CFace*  obj);
    void Delete(CEdge*  obj);
    void Delete(CPoint* obj);
    void Delete(CObject* obj);

    //選択解除
    void SelectedClear();

public slots:

    //Emittor
    DEFINE_EMITTOR(UpdateSelected)
    DEFINE_EMITTOR(UpdatePoints)
    DEFINE_EMITTOR(UpdateEdges)
    DEFINE_EMITTOR(UpdateFaces)
    DEFINE_EMITTOR(UpdateBlocks)
    DEFINE_EMITTOR(UpdateRestraints)
    DEFINE_EMITTOR(UpdateDimensions)
    DEFINE_EMITTOR(UpdateAnyObject)
    DEFINE_EMITTOR(UpdateAnyAction)

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
    void UpdateRestraints();
    void UpdateDimensions();
    void UpdateAnyObject ();
    void UpdateAnyAction ();
};

class CadModelCoreInterface{
protected:
    CadModelCore* model;
public:
    virtual void SetModel(CadModelCore* m){this->model = m;}
};


#endif // CADMODELCORE_H
