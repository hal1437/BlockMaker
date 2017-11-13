#ifndef CADMODELSEARCH_H
#define CADMODELSEARCH_H

#include "CadModelCore.h"

//CadModelCoreに対して探索を行うクラス
class CadModelSearch : public QObject,public CadModelCoreInterface
{
    Q_OBJECT
private:
    //2つの線の共有点を取得
    static CPoint* GetUnionPoint(CEdge* e1,CEdge* e2);
    //2つの線が連続であるか判定
    static bool isContinuity(CEdge* e1,CEdge* e2);
    //線の端点のどちらかが指定した点であるか判定
    static bool HavePoint(CEdge* e1,CPoint* pos);

public:
    //オブジェクト生成補完関数
    QVector<CEdge*> SearchEdgeMakeFace (QVector<CEdge*> select)const;
    QVector<CEdge*> SearchEdgeMakeBlock(QVector<CEdge*> select)const;
    QVector<CFace*> SearchFaceMakeBlock(QVector<CFace*> select)const;

    CadModelSearch();
};

#endif // CADMODELSEARCH_H
