#ifndef CADMODELCORE_H
#define CADMODELCORE_H

#include <QObject>
#include <iostream>
#include <fstream>
#include "CObject/CObject.h"
#include "CObject/CPoint.h"
#include "CObject/CEdge.h"
#include "CObject/CBlock.h"
#include "Dialog/SmartDimension.h"
#include "Restraint.h"



class CadModelCore:public QObject
{
    Q_OBJECT

public:
    //オブジェクト定義
    DEFINE_OBSERVER(CObject*        ,Selected)
    DEFINE_OBSERVER(CEdge*          ,Edges)
    DEFINE_OBSERVER(CBlock*         ,Blocks)
    DEFINE_OBSERVER(Restraint*      ,Restraints)
    DEFINE_OBSERVER(SmartDimension* ,Dimensions)

public:
    CPoint* origin; //原点

public:
    //ファイル入出力
    bool ExportFoamFile(QString filename)const;
    bool ImportFoamFile(QString filename);

public:
    explicit CadModelCore(QWidget *parent = 0);
    ~CadModelCore();

signals:
    void SelectObjectChanged();
    void UpdateSelected  (QVector<CObject*>);
    void UpdateEdges     (QVector<CEdge*>);
    void UpdateBlocks    (QVector<CBlock*>);
    void UpdateRestraints(QVector<Restraint*>);
    void UpdateDimensions(QVector<SmartDimension*>);

};

#endif // CADMODELCORE_H
