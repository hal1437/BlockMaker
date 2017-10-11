#ifndef CADMODELSEARCH_H
#define CADMODELSEARCH_H

#include "CadModelCore.h"

//CadModelCoreに対して探索を行うクラス
class CadModelSearch : public QObject,public CadModelCoreInterface
{
    Q_OBJECT
public:

    //オブジェクト生成補完関数
    QVector<CEdge*> SearchEdgeMakeFace (QVector<CEdge*> edges);
    QVector<CEdge*> SearchFaceMakeBlock(QVector<CEdge*> edges);

    CadModelSearch();
};

#endif // CADMODELSEARCH_H
