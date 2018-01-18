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
    template <class Iterator>
    QList<CPoint*> ConvertChildPoint(Iterator begin,Iterator end)const; //引数の点を取得する
    TRANSFORM_METHOD GetTransformMethod()const;                         //移動タイプ取得

public:
    QVector<CEdge*> translated_edges;//予測オブジェクト
    QVector<CPoint*> translated_pos ;//予測オブジェクト

    void SetModel(CadModelCore *m);

    //変更後の予測位置を描画
    void DrawTranslated(Pos camera,Pos center);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    //移動する関数:Iterator<CObject*>
    template <class Iterator> void AbsoluteMove(Iterator begin,Iterator end, Pos pos);
    template <class Iterator> void RelativeMove(Iterator begin,Iterator end, Pos diff);

    //子の監視
    void ObserveIgnore (CObject* obj); //監視虫
    void ObserveRestart(CObject* obj); //監視再開
    void ObserveStack  (CObject* obj); //監視スタック保存
    void ObservePop    (CObject* obj); //監視スタック解放

    explicit MoveTransformDialog(QWidget *parent = 0);
    ~MoveTransformDialog();

public slots:

    //変更後の予測位置を更新
    void RefreshTranslated();

    void RadioChangedEmitter(bool);   //ラジオボタン変更時スロット
    void ValueChangedEmitter(double); //値変更時スロット

    void Accept();    //適用時
    void Duplicate(); //複製時
    void Closed();    //終了時

signals:
    void RepaintRequest();

};

#endif // MOVETRANSFORMDIALOG_H
