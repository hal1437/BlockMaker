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
    QList<CPoint*   > points;
    QList<CEdge*    > edges ;
    QList<CFace*    > faces ;
    QList<CBlock*   > blocks;
    QList<CStl*     > stls;
    QList<Restraint*> restraints;
    CadModelMenu menu;
    QTreeWidgetItem* rest_root;

protected:
    QIcon   getIcon(CObject* obj);
    QString getName(CObject *obj);
    void mouseReleaseEvent(QMouseEvent* event);

private:
    void AddObjectToTree   (CObject* obj,QTreeWidgetItem* parent,int index);
    void PullSelectedObject(CObject* obj,QTreeWidgetItem* current);
    void PushSelectedObject(CObject* obj,QTreeWidgetItem* current);

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
