#ifndef CADMODELCORE_H
#define CADMODELCORE_H

#include <QObject>
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
    DEFINE_OBSERVER(SmartDimension* ,Dimensions)

public:
    CPoint* origin; //原点

public:
    //ファイル入出力
    bool ExportFoamFile(QString filename)const;
    bool ImportFoamFile(QString filename);

    //選択解除
    bool SelectedClear();

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
    void UpdateAnyAction ();
};

class CadModelCoreInterface{
protected:
    CadModelCore* model;
public:
    virtual void SetModel(CadModelCore* m){this->model = m;}
};


#endif // CADMODELCORE_H
