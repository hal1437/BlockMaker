
#ifndef COBJECT_H
#define COBJECT_H
#include <QPainter>
#include <QDebug>
#include "Utils.h"
#include "Point.h"

class CPoint;

//CADオブジェクト
class CObject
{
public:
    const static int DRAWING_LINE_SIZE = 3; //描画線幅
    static CObject* selecting;              //選択中オブジェクト
    static CObject* createing;              //作成中オブジェクト
    static QVector<CObject*> selected;      //選択オブジェクト
    static QVector<CObject*> all_objects;   //全てのオブジェクト
    static Pos mouse_over;                  //マウス位置

protected:
    bool lock  = false; //ロック
    virtual bool Create(CPoint* pos,int index)=0;

public:

    //型判別
    template<class T>
    bool is()const {
        return (dynamic_cast<const T*>(this) != nullptr);
    }

    //更新関数
    virtual bool Refresh(){return true;}

    //描画関数
    virtual bool Draw(QPainter& painter)const = 0;

    virtual bool isSelecting()const;  //選択中
    virtual bool isSelected() const;  //選択済
    virtual bool isCreating()const;   //作成中
    virtual bool isSelectable()const=0; //選択可能

    //近接点
    virtual Pos GetNear(const Pos& hand)const=0;

    //子オブジェクト取得
    virtual std::vector<CObject*> GetChild(){return std::vector<CObject*>();}

    //座標取得
    virtual int GetJointNum()const = 0;
    virtual Pos GetJointPos(int index)const = 0;
    virtual CPoint* GetJoint(int index) = 0;

    //生成関数
    virtual bool Make(CPoint *pos, int index = 0);
    //移動関数
    virtual bool Move(const Pos& move) = 0;

    //ロック関係
    virtual void Lock(bool lock);
    virtual bool isLock()const;

    //コンストラクタ
    CObject();
    virtual ~CObject();
};




#endif // COBJECT_H
