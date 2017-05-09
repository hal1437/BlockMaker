#ifndef SOLIDVIEW_H
#define SOLIDVIEW_H

#include <QWidget>
#include <QOpenGLWidget>
#include <gl.h>
#include <glu.h>
#include <QOpenGLFunctions>
#include "CObject/CObject.h"
#include "CObject/CPoint.h"
#include "CObject/CEdge.h"
#include "CObject/CLine.h"
#include "CObject/CArc.h"
#include "CObject/CSpline.h"
#include "CObject/CBlock.h"

namespace Ui {
class SolidView;
}

class SolidView : public QOpenGLWidget
{
    Q_OBJECT
private:
    Pos camera;
    Pos center;
    QVector<CEdge*> edges;
    QVector<CBlock> blocks;

public:
    SolidView(QWidget *parent = 0);
    ~SolidView();

protected:
    void initializeGL();        //  OpenGL 初期化
    void resizeGL(int, int);    //  ウィジットリサイズ時のハンドラ
    void paintGL();             //  描画処理

public slots:

    void RefreshUI(QVector<CEdge*> edges,QVector<CBlock> blocks);

};

#endif // SOLIDVIEW_H
