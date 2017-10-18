#ifndef OBJECTLIST_H
#define OBJECTLIST_H

#include <QTreeWidget>
#include <QMouseEvent>
#include <QLabel>
#include <QImage>
#include "CadModelCore.h"
#include "CadModelMenu.h"

class ObjectList : public QTreeWidget, public CadModelCoreInterface
{
    Q_OBJECT

private:
    QList<CPoint*> points;
    QList<CEdge *> edges ;
    QList<CFace *> faces ;
    QList<CBlock*> blocks;
    QList<CStl  *> stls;
    CadModelMenu menu;

protected:
    QIcon getIcon(CObject* obj);
    void mouseReleaseEvent(QMouseEvent* event);

private:
    void AddPointToTree(CPoint* point,QTreeWidgetItem* parent,int index);
    void AddEdgeToTree (CEdge*  edge ,QTreeWidgetItem* parent,int index);
    void AddFaceToTree (CFace*  face ,QTreeWidgetItem* parent,int index);
    void AddBlockToTree(CBlock* block,QTreeWidgetItem* parent,int index);
    void AddStlToTree  (CStl*   stl  ,QTreeWidgetItem* parent,int index);
    void pullSelectedPoint(CPoint* point,QTreeWidgetItem* current);
    void pullSelectedEdge (CEdge*  edge ,QTreeWidgetItem* current);
    void pullSelectedFace (CFace*  face ,QTreeWidgetItem* current);
    void pullSelectedBlock(CBlock* block,QTreeWidgetItem* current);
    void pushSelectedPoint(CPoint* point,QTreeWidgetItem* current);
    void pushSelectedEdge (CEdge*  edge ,QTreeWidgetItem* current);
    void pushSelectedFace (CFace*  face ,QTreeWidgetItem* current);
    void pushSelectedBlock(CBlock* block,QTreeWidgetItem* current);

public:
    void SetModel(CadModelCore* m);

public slots:

    void UpdateObject();    //オブジェクト更新
    void PullSelected();    //選択情報取得
    void PushSelected();    //選択情報同期

public:
    explicit ObjectList(QWidget *parent = 0);
    ~ObjectList();

};

#endif // OBJECTLIST_H
