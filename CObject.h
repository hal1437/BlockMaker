#ifndef COBJECT_H
#define COBJECT_H
#include <QPainter>
#include <QDebug>
#include "Utils.h"
#include "Point.h"
#include "Relative.h"

class CPoint;

//CADオブジェクト
class CObject
{
public:
    const static int DRAWING_LINE_SIZE=3;
    static CObject* selecting;
    static QVector<CObject*> all_objects;
    static QVector<CObject*> selected;
    static Pos mouse_over;
protected:
    bool is_Creating = false;
    bool is_Locking  = false;

    virtual bool Create(CPoint* pos,int index)=0;
public:

    template<class T>
    bool is()const {
        return (dynamic_cast<const T*>(this) != nullptr);
    }

    //更新関数
    virtual bool Refresh(){return true;}
    //描画関数
    virtual bool Draw(QPainter& painter)const = 0;
    //選択関数
    virtual bool Selecting() = 0;
    virtual void Lock(bool lock){is_Locking=lock;}
    //固定関数
    virtual bool isLocked()const{return is_Locking;}
    virtual bool isCreateing()const;
    //移動関数
    virtual bool Move(const Pos& diff)=0;
    //近接点
    virtual Pos GetNear(const Pos& hand)const=0;
    //子オブジェクト取得
    virtual std::vector<CObject*> GetChild(){return std::vector<CObject*>();}

    //座標取得
    virtual int GetJointNum()const=0;
    virtual Pos GetJointPos(int index)const=0;
    virtual CPoint* GetJoint(int index)=0;

    //生成関数
    bool Make(CPoint *pos, int index=0);

    //コンストラクタ
    CObject();
    virtual ~CObject();
};




#endif // COBJECT_H
