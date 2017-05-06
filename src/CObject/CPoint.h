#ifndef CPOINT_H
#define CPOINT_H

#include "CObject.h"

//CAD上の点
class CPoint : public CObject,public Pos
{
    Q_OBJECT
private:
    const static int DRAWING_CIRCLE_SIZE = 5; //描画円半径
    const static int COLLISION_SIZE = 8;      //当たり判定半径

    bool control_point = false; //作用点
    bool moving = false;

public:
    virtual CREATE_RESULT Create(CPoint* pos); //作成関数

    virtual bool Draw(QPainter& painter)const ;//描画関数
    virtual bool Move(const Pos& diff);//移動関数


    virtual bool isLock()const;  //固定点
    virtual bool isControlPoint()const;  //作用点
    virtual bool ControlPoint(bool f);   //作用点設定

    //近接点
    virtual Pos GetNear(const Pos& hand)const;

    //コンストラクタ
    CPoint(QObject* parent=nullptr);
    CPoint(const Pos& origin);
    CPoint(const Pos& pos,QObject* parent);
    CPoint(double x,double y,QObject* parent=nullptr);//初期Create済みスターターセット
    ~CPoint();

    CPoint& operator=(const Pos& rhs);

signals:
    //座標移動シグナル
    void PosChanged(Pos new_pos,Pos old_pos);

public slots:

};

#endif // CPOINT_H
