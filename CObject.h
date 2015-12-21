#ifndef COBJECT_H
#define COBJECT_H
#include <QPainter>
#include "Point.h"
#include "Relative.h"


//CADオブジェクト
class CObject
{
public:
    static CObject* select_obj;
protected:
    bool is_Creating;
    bool selecting = false;

    virtual bool Create(Relative<Pos> pos,int index)=0;
public:
    static Pos mouse_over;

    //描画関数
    virtual bool Draw(QPainter& painter)const = 0;
    //選択関数
    virtual bool Selecting() = 0;
    //固定関数
    virtual bool isLocked()=0;

    //生成関数
    bool Make(Pos& pos,int index=0);
    bool Make(Pos&& pos, int index=0);
    bool Make(Relative<Pos>& pos, int index=0);

    //コンストラクタ
    CObject();
    virtual ~CObject();
};

#endif // COBJECT_H
