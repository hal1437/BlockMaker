
#ifndef COBJECT_H
#define COBJECT_H
#include <QPainter>
#include <QTransform>
#include <QDebug>
#include "Point.h"

//OpenGLのインクルード
#ifdef MACOSX
#include <OpenGL.h>
#include <GLUT/glut.h>
#elif UNIX
#include <GL/gl.h>
#include <GL/glu.h>
#endif

class CPoint;

enum CREATE_RESULT{
    COMPLETE  = 0,//一回で終了
    ONESHOT   = 1,//一回で終了
    TWOSHOT   = 2,//二回で終了
    THREESHOT = 3,//三回で終了
    ENDLESS   = -1,//何回でも可能
};


//CADオブジェクト
class CObject:public QObject
{
    Q_OBJECT
public:
    static constexpr double COLLISION_SIZE = 6; //当たり判定半径
    static constexpr double DRAWING_LINE_SIZE = 3; //描画線幅
    static constexpr double SAME_POINT_EPS = 0.001;    //同一点誤差

    static double drawing_scale;           //描画スケール

    //フラグ定義
    DEFINE_FLAG(Lock   ,false)
    DEFINE_FLAG(Visible,true)

public:

    //型判別
    template<class T>
    bool is()const {
        return (dynamic_cast<const T*>(this) != nullptr);
    }

    virtual bool DrawGL(Pos camera,Pos center)const = 0; //三次元描画関数
    virtual bool Move  (const Pos& diff) = 0;            //移動関数

    virtual bool isSelectable(Pos pos)const;  //posの位置で選択可能か

    //近接点
    virtual Pos GetNearPos (const Pos& hand)const=0;
    virtual Pos GetNearLine(const Pos& pos1,const Pos& pos2)const=0;

    //複製
    virtual  CObject* Clone()const = 0;

    //コンストラクタ
    explicit CObject(QObject* parent=nullptr);
    virtual ~CObject();
};




#endif // COBJECT_H
