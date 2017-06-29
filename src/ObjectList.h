#ifndef OBJECTLIST_H
#define OBJECTLIST_H

#include <QTreeWidget>
#include <QMouseEvent>
#include <QMenu>
#include "CadModelCore.h"

class ObjectList : public QTreeWidget, public CadModelCoreInterface
{
    Q_OBJECT

private:
    QVector<CPoint*> points;
    QVector<CEdge *> edges ;
    QVector<CFace *> faces ;
    QVector<CBlock*> blocks;
    QMenu* menu;
    QAction* delete_action;
    QAction* visible_action;
    QAction* invisible_action;
    QAction* visible_frame_action;
    QAction* invisible_frame_action;

protected:
    void mouseReleaseEvent(QMouseEvent* event);

private:
    void AddBlockToTree(CBlock* block,QTreeWidgetItem* parent,int index);
    void AddFaceToTree (CFace*  face ,QTreeWidgetItem* parent,int index);
    void AddEdgeToTree (CEdge*  edge ,QTreeWidgetItem* parent,int index);
    void AddPointToTree(CPoint* point,QTreeWidgetItem* parent,int index);
    void pullSelectedBlock(CBlock* block,QTreeWidgetItem* current);
    void pullSelectedFace (CFace*  face ,QTreeWidgetItem* current);
    void pullSelectedEdge (CEdge*  edge ,QTreeWidgetItem* current);
    void pullSelectedPoint(CPoint* point,QTreeWidgetItem* current);
    void pushSelectedBlock(CBlock* block,QTreeWidgetItem* current);
    void pushSelectedFace (CFace*  face ,QTreeWidgetItem* current);
    void pushSelectedEdge (CEdge*  edge ,QTreeWidgetItem* current);
    void pushSelectedPoint(CPoint* point,QTreeWidgetItem* current);

public:
    void SetModel(CadModelCore* m);

public slots:

    void UpdateObject();    //オブジェクト更新
    void PullSelected();    //選択情報取得
    void PushSelected();    //選択情報同期

    void SetVisible(bool);//可視設定
    void SetInvisible(bool);//不可視設定
    void SetVisibleFrame(bool);//可視フレーム設定
    void SetInvisibleFrame(bool);//不可視フレーム設定

public:
    explicit ObjectList(QWidget *parent = 0);
    ~ObjectList();

};

#endif // OBJECTLIST_H
