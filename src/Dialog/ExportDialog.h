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

class ExportDialog : public QDialog ,public CadModelCoreInterface
{
    Q_OBJECT
private:
    void Export(QString filename)const;

    //座標から番号へ変換
    int GetPosIndex(CPoint *p)const;

    //境界面の座標を取得
    QVector<CPoint *> GetBoundaryPos(CBlock *block, BoundaryDir dir)const;

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
