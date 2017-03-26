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

struct VPos{
    double x,y,z;
    bool operator==(const VPos& rhs)const{
        return std::tie(this->x,this->y,this->z) == std::tie(rhs.x,rhs.y,rhs.z);
    }
};

namespace Ui {
class ExportDialog;
}

class ExportDialog : public QDialog
{
    Q_OBJECT
private:
    QVector<CObject*> objects;
    QVector<CBlock> blocks;
    void Export(QString filename)const;

    //座標から番号へ変換
    int GetPosIndex(VPos p)const;

    //全頂点リストを取得
    QVector<VPos> GetVerticesPos()const;

public:
    void SetBlocks(QVector<CBlock> blocks);

    explicit ExportDialog(QWidget *parent = 0);
    ExportDialog(QVector<CObject *> objects, QWidget *parent = 0);
    ~ExportDialog();

public slots:
    void ChangeDirctory();
    void AcceptDialog();

private:
    Ui::ExportDialog *ui;
};

#endif // EXPORTDIALOG_H
