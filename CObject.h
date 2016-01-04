#ifndef COBJECT_H
#define COBJECT_H
#include <QPainter>
#include "Utils.h"
#include "Point.h"
#include "Relative.h"

//CADオブジェクト
class CObject
{
public:
    static CObject* selecting;
    static std::vector<CObject*> selected;
    static Pos mouse_over;
protected:
    bool is_Creating;

    virtual bool Create(Relative<Pos> pos,int index)=0;
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
    //固定関数
    virtual bool isLocked()=0;
    virtual bool isCreateing()const;
    //移動関数
    virtual bool Move(const Pos& diff)=0;
    //近接点
    virtual Pos GetNear(const Pos& hand)const=0;

    //座標取得
    virtual int GetJointNum()const=0;
    virtual Pos GetJointPos(int index)const=0;

    //生成関数
    bool Make(Pos& pos,int index=0);
    bool Make(Pos&& pos, int index=0);
    bool Make(Relative<Pos>& pos, int index=0);

    //コンストラクタ
    CObject();
    virtual ~CObject();
};




#endif // COBJECT_H
