#ifndef MOVETRANSFORMDIALOG_H
#define MOVETRANSFORMDIALOG_H

#include <QDialog>
#include <QKeyEvent>
#include "CadModelCore.h"

namespace Ui {
class MoveTransformDialog;
}

class MoveTransformDialog : public QDialog,public CadModelCoreInterface
{
    Q_OBJECT

private:
    Ui::MoveTransformDialog *ui;

    //移動タイプ
    enum TRANSFORM_METHOD{
        RELATIVE, //相対
        ABSOLUTE  //絶対
    };
private:
    QVector<CPoint*> ConvertChildPoint(QVector<CObject*> objects)const; //引数の点を取得する
    TRANSFORM_METHOD GetTransformMethod()const;                        //移動タイプ取得

public:
    QVector<CObject*> translated;//予測オブジェクト

    void SetModel(CadModelCore *m);

    //変更後の予測位置を描画
    void DrawTranslated(Pos camera,Pos center);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void AbsoluteMove(QVector<CObject*> objects, Pos pos);
    void RelativeMove(QVector<CObject*> objects, Pos diff);

    explicit MoveTransformDialog(QWidget *parent = 0);
    ~MoveTransformDialog();

public slots:
    void PauseChanged();
    void RestartChanged();

    //変更後の予測位置を表示
    void RefreshTranslated();

    void RadioChangedEmitter(bool);
    void ValueChangedEmitter(double);

    void Accept();
    void Duplicate();
    void Closed();
//    void UpdateObjects(); //オブジェクト更新

signals:
    void RepaintRequest();

};

#endif // MOVETRANSFORMDIALOG_H
