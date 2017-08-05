#ifndef CADMODELMENU_H
#define CADMODELMENU_H

#include <QMenu>
#include "CadModelCore.h"

class CadModelMenu : public QObject,public CadModelCoreInterface
{
    Q_OBJECT

private:
    QMenu* menu;
    QAction* delete_action;
    QAction* reverse_action;
    QAction* visible_action;
    QAction* invisible_action;
    QAction* visible_mesh_action;
    QAction* invisible_mesh_action;

public:
    //メニュー表示
    void Show(QPoint pos);

    explicit CadModelMenu(QObject *parent = 0);

public slots:

    void Delete(bool);//削除
    void ReverseArc(bool);//削除
    void SetVisible(bool);//可視設定
    void SetInvisible(bool);//不可視設定
    void SetVisibleMesh(bool);//可視フレーム設定
    void SetInvisibleMesh(bool);//不可視フレーム設定
};

#endif // CADMODELMENU_H
