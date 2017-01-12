#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <CObject/CObject.h>
#include <CObject/CPoint.h>
#include <CObject/CLine.h>
#include <CObject/CArc.h>
#include <CObject/CSpline.h>
#include <CObject/CBlocks.h>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextCodec>

struct VPos{
    double x,y,z;
};

namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT
private:
    std::vector<CObject*> objects;
    std::vector<CBlocks> blocks;

public:
    void SetBlocks(std::vector<CBlocks> blocks);

    explicit ExportDialog(QWidget *parent = 0);
    ExportDialog(std::vector<CObject*> objects,QWidget *parent = 0);
    ~ExportDialog();

public slots:
    void ChangeDirctory();
    void Export(QString filename)const;
    void AcceptDialog();

private:
    Ui::ExportDialog *ui;
};

#endif // EXPORTDIALOG_H
