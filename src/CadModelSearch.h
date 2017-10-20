#ifndef CADMODELSEARCH_H
#define CADMODELSEARCH_H

#include "CadModelCore.h"

//CadModelCoreに対して探索を行うクラス
class CadModelSearch : public QObject,public CadModelCoreInterface
{
    Q_OBJECT
private:

    static CPoint* GetUnionPoint(CEdge* e1,CEdge* e2); //共有点を取得
    static bool isContinuity(CEdge* e1,CEdge* e2);     //2つの線が連続であるか
public:

    //オブジェクト生成補完関数
    QVector<CEdge*> SearchEdgeMakeFace (QVector<CEdge*> select);
    QVector<CEdge*> SearchFaceMakeBlock(QVector<CEdge*> select);

    CadModelSearch();
};

#endif // CADMODELSEARCH_H
