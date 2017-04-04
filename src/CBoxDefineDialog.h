#ifndef CBOXDEFINEDIALOG_H
#define CBOXDEFINEDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include "CObject/CBlock.h"

namespace Ui {
class CBoxDefineDialog;
}


class CBoxDefineDialog : public QDialog
{
    Q_OBJECT
protected:
    void paintEvent    (QPaintEvent* event); //描画イベントハンドラ
    QComboBox*   ConvertDirToCombo      (BoundaryDir  dir)const;
    QLineEdit*   ConvertDirToNameEdit   (BoundaryDir  dir)const;
    QString      ConvertBoundaryToString(BoundaryType dir)const;
    BoundaryType ConvertStringToBoundary(QString      str)const;
    QString      ConvertGradingToString (GradingType  dir)const;
    GradingType  ConvertStringToGrading (QString      str)const;

public:

    //UI操作
    QString      GetBoundaryName(BoundaryDir dir)const;
    BoundaryType GetBoundaryType(BoundaryDir dir)const;
    GradingType  GetGradigngType()const;
    QString      GetGradigngArgs()const;

    void SetGradigngType(GradingType type);
    void SetBoundaryName(BoundaryDir dir,QString name     )const;
    void SetBoundaryType(BoundaryDir dir,BoundaryType type)const;
    QString      GetVertices    (BoundaryDir dir)const;

    //エラー判定
    bool isFormatError()const;

    //入出力
    CBlock ExportCBlock()const;
    void    ImportCBlock(const CBlock& block);



    explicit CBoxDefineDialog(QWidget *parent = 0);
    ~CBoxDefineDialog();

public slots:
    void AcceptProxy();
    void GradigngComboChanged(QString text);

private:
    Ui::CBoxDefineDialog *ui;

};




#endif // CBOXDEFINEDIALOG_H
