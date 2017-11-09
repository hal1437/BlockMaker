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
    QLabel    face_boundary_label;
    QComboBox face_boundary_combo;
    QPushButton face_boundary_button;

    //Edge
    QLabel    edge_divide_label;
    QSpinBox  edge_divide_spin;
    QLabel    edge_grading_label;
    QDoubleSpinBox  edge_grading_spin;

private:
    void ConstructFace();//Face用レイアウト定義
    void ConstructEdge();//Edge用レイアウト定義

public:

    bool CheckAvailable()const;
    void SetModel(CadModelCore *m);

    explicit PropertyDefinitionDialog(QWidget *parent = 0);
    ~PropertyDefinitionDialog();

public slots:
    void ShowBoundayDefinitionDialog();
    void UpdateLayout();
    void Accept();

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
