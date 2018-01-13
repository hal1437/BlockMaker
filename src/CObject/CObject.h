
#ifndef COBJECT_H
#define COBJECT_H
#include <QPainter>
#include <QTransform>
#include <QDebug>
#include <Conflict.h>
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
    static constexpr double  COLLISION_SIZE = 6;      //当たり判定半径
    static constexpr double  DRAWING_LINE_SIZE = 3;   //描画線幅
    static constexpr double  SAME_POINT_EPS = 1.0e-8; //同一点誤差
    static constexpr double  SAME_ANGLE_EPS = 1.0e-8; //同一点誤差
    static double            drawing_scale;           //描画スケール

public:

    //名前
    OBSERVE_MEMBER(QString,Name,name)

    //フラグ定義
    bool              observe_pause; //子の変更による更新を停止
    QVector<CObject*> observe_queue; //子変更キュー

    DEFINE_FLAG(Lock   ,false)       //固定
    DEFINE_FLAG(Visible,true)        //表示
    DEFINE_FLAG(VisibleDetail,false)  //分割フレーム表示

    DEFINE_EMITTOR_ARG(Changed,this) // 変更エミッター

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

    virtual QString DefaultClassName(){return "CObject";}

    virtual void DrawGL(Pos camera,Pos center)const = 0; //三次元描画関数
    virtual bool isSelectable(Pos pos)const;     //posの位置で選択可能か

    //子の操作
    virtual CObject* GetChild     (int index) = 0;
    virtual CObject* GetChild     (int index)const;
    virtual void     SetChild     (int index,CObject* obj)=0;
    virtual void     InsertChild  (int index,CObject* obj);
    virtual int      GetChildCount()const = 0;
    virtual QVector<CPoint*> GetAllChildren()const;

    //最近傍点の取得
    virtual Pos GetNearPos (const Pos& hand)const=0;
    virtual Pos GetNearLine(const Pos& pos1,const Pos& pos2)const=0;

    //複製
    virtual  CObject* Clone()const = 0;
    //更新
    virtual  void Refresh();

    //子監視関連
    void ObservePause  ();//監視を一時停止
    void ObserveRestart();//監視を再開

    //コンストラクタ
    explicit CObject(QObject* parent=nullptr);
    virtual ~CObject();

signals:
    //移動シグナル
    void Changed();
    void Changed(CObject* child);

    //競合/解決シグナル
    void Conflicted(CObject* object,Conflict conf);
    void Solved    (CObject* object);

public slots:
    //変更コールバック保存
    void StackChangeCallback(CObject* children);
    //子変更コールバック
    virtual void ChangeChildCallback(QVector<CObject*> children);
};




#endif // COBJECT_H
