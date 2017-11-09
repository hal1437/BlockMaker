#ifndef BOUNDARYDEFINITIONDIALOG_H
#define BOUNDARYDEFINITIONDIALOG_H

#include "CadModelCore.h"
#include <QDialog>

#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>


class BoundaryDefinitionDialog : public QDialog, public CadModelCoreInterface{
    Q_OBJECT
public:
    static QVector<Boundary> boundary_list;//境界面リスト
    QVector<QString> boundary_combo_text = {"empty (2次元)","patch (パッチ)","wall (壁)",
                                            "symmetryPlane (対称面)","cyclic (周期境界)",
                                            "cyclicAMI (不整合周期境界)","wedge (2次元軸対称)",
                                            "Undefined (未定義)"};
public:
    QTableWidget*  table;
    QPushButton* plus ;
    QPushButton* minus;

    void SetModel(CadModelCore *m);
    BoundaryDefinitionDialog();

public slots:
    void SaveTable();//テーブル保存
    void LoadTable();//テーブル読み出し
    void PlusButtonPushed();
    void MinusButtonPushed();
    void ComboChanged(int);
    void CellChanged(int,int);
};

#endif // BOUNDARYDEFINITIONDIALOG_H
