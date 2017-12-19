#ifndef CONFLICTLIST_H
#define CONFLICTLIST_H

#include <QObject>
#include <CObject/CObject.h>
#include <functional>
#include <CadModelCore.h>
#include <QTreeWidget>
#include <QPushButton>

class ConflictList:public QObject, CadModelCoreInterface
{
    Q_OBJECT
public:
    QVector<std::pair<CObject*,Conflict>> conflicts;
    QTreeWidget* table;

private:
    //再描画
    void Repaint();

public:
    //モデルの競合を自動的に追加
    void SetModel(CadModelCore *m);

    ConflictList();

public slots:
    //Conflict追加
    void AddConflict(CObject* obj,Conflict conf);
    void RemoveConflict(CObject* obj);

    //テーブルセット
    void SetWidget(QTreeWidget* table);

signals:
    //変更済み
    void Changed();
};

#endif // CONFLICTLIST_H
