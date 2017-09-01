#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <CObject/CObject.h>
#include <CObject/CPoint.h>
#include <CObject/CLine.h>
#include <CObject/CArc.h>
#include <CObject/CSpline.h>
#include <CObject/CBlock.h>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextCodec>
#include <QSettings>
#include "CadModelCore.h"
#include "FoamFile.h"

namespace Ui {
class ExportDialog;
}

//面の方向
enum BoundaryDir{
    Top,
    Right,
    Left,
    Bottom,
    Front,
    Back,
};

class ExportDialog : public QDialog ,public CadModelCoreInterface
{
    Q_OBJECT
private:
    void Export(QString filename)const;

    //座標から番号へ変換
    int GetPosIndex(CPoint *p)const;

    //方向から面を取得し適切な順番に点を並び替えて返す
    QVector<CPoint*> GetBoundaryPos(CBlock* block,BoundaryDir dir)const;

public:
    explicit ExportDialog(QWidget *parent = 0);
    ~ExportDialog();

public slots:
    void ChangeDirctory();
    void AcceptDialog();

private:
    Ui::ExportDialog *ui;
};

#endif // EXPORTDIALOG_H
