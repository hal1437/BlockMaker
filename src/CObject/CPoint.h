#ifndef CPOINT_H
#define CPOINT_H

#include "CObject.h"

//三次元座標上の点
class CPoint : public CObject,public Pos
{
    Q_OBJECT
private:
    const static int DRAWING_CIRCLE_SIZE = 5; //描画円半径
public:
    const static int COLLISION_SIZE = 8;      //当たり判定半径
    bool control_point = false; //作用点
    bool moving = false;

public:

    virtual CREATE_RESULT Create(CPoint* pos); //作成関数

    virtual bool DrawGL(Pos camera,Pos center)const; //三次元描画関数
    virtual bool Move  (const Pos& diff);              //移動関数

    virtual bool isLock()const;  //固定点
    virtual bool isControlPoint()const;  //作用点
    virtual bool ControlPoint(bool f);   //作用点設定

    //近接点
    virtual Pos GetNearPos (const Pos& hand)const;
    virtual Pos GetNearLine(const Pos& pos1,const Pos& pos2)const;

    //複製
    virtual CObject* Clone()const;

    //コンストラクタ
    CPoint(QObject* parent=nullptr);
    CPoint(const Pos& origin);
    CPoint(const Pos& pos,QObject* parent);
    CPoint(double x,double y,double z = 0,QObject* parent=nullptr);//初期Create済みコンストラクタ
    ~CPoint();

    CPoint& operator=(const Pos& rhs);

signals:
    //座標移動シグナル
    void PosChanged();
    void PosChanged(CPoint* pos,Pos old_pos);

public slots:

};

#endif // CPOINT_H
