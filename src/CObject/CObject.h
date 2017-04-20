
#ifndef COBJECT_H
#define COBJECT_H
#include <QPainter>
#include <QTransform>
#include <QDebug>
#include "Utils.h"
#include "Point.h"

class CPoint;

//CADオブジェクト
class CObject:public QObject
{
public:
    static constexpr double COLLISION_SIZE = 6; //当たり判定半径
    static constexpr double DRAWING_LINE_SIZE = 3; //描画線幅
    static constexpr double SAME_POINT_EPS = 0.001;    //同一点誤差

    static double drawing_scale;           //描画スケール
    static CObject* hanged;                //マウス直下のオブジェクト
    static CObject*  creating;             //作成中オブジェクト
    static QVector<CObject*> selected;     //選択オブジェクト
    static Pos mouse_pos;                  //マウス位置

protected:
    bool lock  = false; //ロック状態

public:

    //型判別
    template<class T>
    bool is()const {
        return (dynamic_cast<const T*>(this) != nullptr);
    }

    //作成関数(完了時:true , 継続時:false)
    virtual bool Create(CPoint* start,CPoint* end) = 0;

    virtual bool Draw(QPainter& painter)const = 0;//描画関数
    virtual bool Move(const Pos& diff) = 0;//移動関数
    virtual void Lock(bool lock);//ロック

    virtual bool isSelecting() const;  //選択中
    virtual bool isSelected()  const;  //選択済
    virtual bool isCreating()  const;  //作成中
    virtual bool isLock()      const;  //固定中
    virtual bool isSelectable(Pos pos)const;  //mouse_posの位置で選択可能か

    //近接点
    virtual Pos GetNear(const Pos& hand)const=0;

    //コンストラクタ
    CObject(QObject* parent=nullptr);
    virtual ~CObject();
};




#endif // COBJECT_H
