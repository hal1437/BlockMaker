#ifndef PROPERTYDEFINITIONDIALOG_H
#define PROPERTYDEFINITIONDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>
#include <CadModelCore.h>
#include <CObject/CEdge.h>
#include <CObject/CFace.h>
#include <Dialog/BoundaryDefinitionDialog.h>

namespace Ui {
class PropertyDefinitionDialog;
}

class PropertyDefinitionDialog : public QDialog,public CadModelCoreInterface
{
    Q_OBJECT
private:
    enum class CONSTRUCTED{
        EDGE,
        FACE,
        EMPTY
    };


    CONSTRUCTED constructed = CONSTRUCTED::EMPTY;//表示タイプ

    //All
    QLabel    name_label;
    QLineEdit name_edit;

    //Face
    QLabel      face_boundary_label;
    QComboBox   face_boundary_combo;
    QPushButton face_boundary_button;

    //Edge
    QLabel             edge_all_divide_label;
    QTableWidget       edge_multi_grading_table;  //MultiGradingテーブル
    QPushButton        edge_add_grading_button;   //MultiGrading行追加ボタン
    QPushButton        edge_remove_grading_button;   //MultiGrading行削除ボタン
    QSpinBox           edge_all_divide_spin;      //総分割数
    QVector<QDoubleSpinBox*> edge_dir_spins;      //長さ割合
    QVector<QDoubleSpinBox*> edge_divide_spins;   //分割数割合
    QVector<QDoubleSpinBox*> edge_grading_spins;  //分割寄せ割合

private:
    void ConstructFace();//Face用レイアウト定義
    void ConstructEdge();//Edge用レイアウト定義

    void SetupDoubleSpin(QDoubleSpinBox* dir,QDoubleSpinBox* divide,QDoubleSpinBox* grading)const;

public:

    bool CheckAvailable()const;
    void SetModel(CadModelCore *m);

    explicit PropertyDefinitionDialog(QWidget *parent = 0);
    ~PropertyDefinitionDialog();

public slots:
    void ShowBoundayDefinitionDialog();
    void UpdateLayout();
    void Accept();
    void AddMultiGradingTable();
    void RemoveMultiGradingTable();

    void Changed();
    void LineEditChanged(QString);
    void SpinChanged(int);
    void DoubleSpinChanged(double);
    void ComboChanged(int);

private:
    Ui::PropertyDefinitionDialog *ui;

signals:
    void RepaintRequest();
};

#endif // PROPERTYDEFINITIONDIALOG_H
