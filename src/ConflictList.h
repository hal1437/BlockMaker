#ifndef CONFLICTLIST_H
#define CONFLICTLIST_H

#include <QObject>
#include <CObject/CObject.h>
#include <functional>
#include <CadModelCore.h>

class ConflictList:public QObject ,public CadModelCoreInterface
{
    Q_OBJECT
public:
    QVector<std::pair<CObject*,Conflict>> conflicts;

public:
    //モデルの競合を自動的に追加
    void SetModel(CadModelCore *m);

    ConflictList();

public slots:
    //Conflict追加
    void AddConflict(CObject* obj,Conflict conf);
};

#endif // CONFLICTLIST_H
