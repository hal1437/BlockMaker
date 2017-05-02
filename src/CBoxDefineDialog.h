#ifndef CBOXDEFINEDIALOG_H
#define CBOXDEFINEDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include "CObject/CBlock.h"

namespace Ui {
class CBoxDefineDialog;
}


class CBoxDefineDialog : public QDialog
{
    Q_OBJECT
private:
    QVector<QDoubleSpinBox*> grading_args;
    BoundaryType types_log[6];

protected:
    QComboBox*   ConvertDirToCombo      (BoundaryDir  dir)const;
    QLineEdit*   ConvertDirToNameEdit   (BoundaryDir  dir)const;
    QString      ConvertBoundaryToString(BoundaryType type)const;
    BoundaryType ConvertStringToBoundary(QString      str)const;
    QString      ConvertGradingToString (GradingType  dir)const;
    GradingType  ConvertStringToGrading (QString      str)const;

public:

    //UI操作
    QString      GetBoundaryName(BoundaryDir dir)const;
    BoundaryType GetBoundaryType(BoundaryDir dir)const;
    GradingType  GetGradigngType()const;

    void SetGradigngType(GradingType type);
    void SetBoundaryName(BoundaryDir dir,QString name     );
    void SetBoundaryType(BoundaryDir dir,BoundaryType type);
    QString GetVertices (BoundaryDir dir)const;

    //接続モード
    void ConnectionLock(BoundaryDir dir,bool lock);

    //エラー判定
    QString FormatError()const;

    //入出力
    CBlock ExportCBlock()const;
    void   ImportCBlock(const CBlock& block);

    explicit CBoxDefineDialog(QWidget *parent = 0);
    ~CBoxDefineDialog();

public slots:
    void AcceptProxy();
    void GradigngComboChanged(QString text);


    //境界名と境界タイプを一致させる
    void SyncOtherCombo(int);

private:
    Ui::CBoxDefineDialog *ui;

};




#endif // CBOXDEFINEDIALOG_H
