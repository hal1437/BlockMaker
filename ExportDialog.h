#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <CObject.h>
#include <CPoint.h>
#include <CLine.h>
#include <CArc.h>
#include <CSpline.h>
#include <CBlocks.h>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QTextCodec>

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
