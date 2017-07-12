#ifndef CBOXDEFINEDIALOG_H
#define CBOXDEFINEDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <CadModelCore.h>
#include "CObject/CBlock.h"

#define DEFINE_BOX_DIALOG_NAME_SLOTS(DIR,INDEX)   \
    void Set##DIR##Name(QString name){            \
        this->block->name[INDEX] = name;          \
    }
#define DEFINE_BOX_DIALOG_TYPE_SLOTS(DIR,INDEX)                       \
    void Set##DIR##Type(QString name){                                \
        this->block->boundery[INDEX] = this->ConvertStringToBoundary(name); \
    }


namespace Ui {
class CBoxDefineDialog;
}


class CBoxDefineDialog : public QDialog,public CadModelCoreInterface
{
    Q_OBJECT

public:
    CBlock* block;

private:
    Ui::CBoxDefineDialog *ui;
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

    //面取得
    CFace* GeFaceFormDir(BoundaryDir dir);
    bool GetFaceContinuous(BoundaryDir dir)const;

    //エラー判定
    QString FormatError()const;
    void ExportCBlock();
    void ImportCBlock();

    explicit CBoxDefineDialog(QWidget *parent = 0);
    ~CBoxDefineDialog();

public slots:
    void AcceptProxy();
    void GradigngComboChanged(QString text);

    DEFINE_BOX_DIALOG_NAME_SLOTS(Up    ,0)
    DEFINE_BOX_DIALOG_NAME_SLOTS(Right ,1)
    DEFINE_BOX_DIALOG_NAME_SLOTS(Left  ,2)
    DEFINE_BOX_DIALOG_NAME_SLOTS(Down  ,3)
    DEFINE_BOX_DIALOG_NAME_SLOTS(Front ,4)
    DEFINE_BOX_DIALOG_NAME_SLOTS(Back  ,5)
    DEFINE_BOX_DIALOG_TYPE_SLOTS(Up    ,0)
    DEFINE_BOX_DIALOG_TYPE_SLOTS(Right ,1)
    DEFINE_BOX_DIALOG_TYPE_SLOTS(Left  ,2)
    DEFINE_BOX_DIALOG_TYPE_SLOTS(Down  ,3)
    DEFINE_BOX_DIALOG_TYPE_SLOTS(Front ,4)
    DEFINE_BOX_DIALOG_TYPE_SLOTS(Back  ,5)



    //境界名と境界タイプを一致させる
    void SyncOtherCombo(int);


};




#endif // CBOXDEFINEDIALOG_H
