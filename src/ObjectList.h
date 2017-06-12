#ifndef OBJECTLIST_H
#define OBJECTLIST_H

#include <QTreeWidget>
#include "CadModelCore.h"

namespace Ui {
class ObjectList;
}

class ObjectList : public QTreeWidget, public CadModelCoreInterface
{
    Q_OBJECT

public:
    void SetModel(CadModelCore* m);

public slots:
    void UpdateObject  ();
    void UpdateSelected();

public:
    explicit ObjectList(QWidget *parent = 0);
    ~ObjectList();

};

#endif // OBJECTLIST_H
