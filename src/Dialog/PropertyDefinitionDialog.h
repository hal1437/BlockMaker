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

    QVector<QString> boundary_combo_text = {"patch (パッチ)","wall (壁)","symmetryPlane (対称面)",
                                            "cyclic (周期境界)","cyclicAMI (不整合周期境界)",
                                            "wedge (2次元軸対称)","empty (2次元)","None (連続)",""};
    CONSTRUCTED constructed = CONSTRUCTED::EMPTY;

    //Face
    QLabel    face_name_label;
    QLineEdit face_name_edit;
    QLabel    face_boundary_label;
    QComboBox face_boundary_combo;

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
    void UpdateLayout();
    void Accept();

private:
    Ui::PropertyDefinitionDialog *ui;

signals:
    void RepaintRequest();
};

#endif // PROPERTYDEFINITIONDIALOG_H
