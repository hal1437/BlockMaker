#ifndef CBOXDEFINEDIALOG_H
#define CBOXDEFINEDIALOG_H

#include <QDialog>
#include "CBlocks.h"

namespace Ui {
class CBoxDefineDialog;
}


class CBoxDefineDialog : public QDialog
{
    Q_OBJECT
protected:
    void paintEvent    (QPaintEvent* event); //描画イベントハンドラ
public:

    //面の情報を取得
    QString      GetBoundaryName(BoundaryDir dir)const;
    BoundaryType GetBoundaryType(BoundaryDir dir)const;
    QString      GetBoundaryArgs(BoundaryDir dir)const;
    QString      GetVertices    (BoundaryDir dir)const;
    GradingType  GetGradigngType()const;
    QString      GetGradigngArgs()const;

    //エラー判定
    bool isFormatError()const;

    //出力
    CBlocks ExportCBlocks()const;

    explicit CBoxDefineDialog(QWidget *parent = 0);
    ~CBoxDefineDialog();

public slots:
    void AcceptProxy();

private:
    Ui::CBoxDefineDialog *ui;

};




#endif // CBOXDEFINEDIALOG_H
