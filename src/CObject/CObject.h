
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
    static constexpr double SAME_POINT_EPS = 0.001;//同一点誤差
    static double drawing_scale;           //描画スケール

    //名前
    OBSERVE_MEMBER(QString,Name)

    //フラグ定義
    DEFINE_FLAG(Lock   ,false)
    DEFINE_FLAG(Visible,true)    

protected:
    //子を監視対象にする
    void ObserveChild(CObject* obj);
    void IgnoreChild (CObject* obj);

public:
    //型判別
    template<class T>
    bool is()const {
        return (dynamic_cast<const T*>(this) != nullptr);
    }

    virtual void DrawGL(Pos camera,Pos center)const = 0; //三次元描画関数
    //virtual void MoveAbsolute(const Pos& diff);  //絶対移動関数
    //virtual void MoveRelative(const Pos& diff);  //相対移動関数
    virtual bool isSelectable(Pos pos)const;     //posの位置で選択可能か

    //子の操作
    virtual CObject* GetChild     (int index) = 0;
    virtual CObject* GetChild     (int index)const;
    virtual void     SetChild     (int index,CObject* obj)=0;
    virtual void     InsertChild  (int index,CObject* obj);
    virtual int      GetChildCount()const = 0;
    virtual QVector<CPoint*> GetAllChildren();

    //近接点
    virtual Pos GetNearPos (const Pos& hand)const=0;
    virtual Pos GetNearLine(const Pos& pos1,const Pos& pos2)const=0;

    //複製
    virtual  CObject* Clone()const = 0;

    //コンストラクタ
    explicit CObject(QObject* parent=nullptr);
    virtual ~CObject();

signals:
    //移動シグナル
    void Changed();
    void Changed(CObject* child);

public slots:
    //子変更コールバック
    virtual void ChangeChildCallback(CObject* child);

protected slots:
    //引数有りから無しを呼び出す
    void ChangeChildHandler(CObject* obj);
};




#endif // COBJECT_H
