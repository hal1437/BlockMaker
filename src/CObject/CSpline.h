#ifndef CSPLINE_H
#define CSPLINE_H

#include "CObject.h"
#include "CPoint.h"
#include <memory>

#define MaxSplineSize 100
class Spline {
public:
    int num=0;
    double a[MaxSplineSize+1], b[MaxSplineSize+1], c[MaxSplineSize+1], d[MaxSplineSize+1];
public:
    Spline(){}
    void init(std::vector<double> sp);
    double culc(double t)const;
};

//CAD上の点
class CSpline : public CObject
{
private:
    const static int DIVISION = 10;//分解数
    const static int DRAWING_LINE_SIZE = 3;//線の太さ
    const static int COLLISION_SIZE = 6;//あたり判定の大きさ
protected:
    std::vector<CPoint*> pos;
    Spline xs;
    Spline ys;

public:

    //作成関数(完了時:true , 継続時:false)
    virtual bool Create(CPoint* pos,int index);

    //更新関数
    virtual bool Refresh();

    virtual bool Draw(QPainter& painter)const;//描画関数
    virtual bool Move(const Pos& move);//移動関数
    virtual void Lock(bool lock);//ロック

    //virtual bool isSelecting() const;  //選択中
    //virtual bool isSelected()  const;  //選択済
    //virtual bool isCreating()  const;  //作成中
    //virtual bool isLock()      const;  //固定中
    virtual bool isSelectable()const;  //mouse_posの位置で選択可能か

    //近接点
    virtual Pos GetNear(const Pos& hand)const;

    //ジョイント関係
    virtual int     GetJointNum()         const;
    virtual Pos     GetJointPos(int index)const;
    virtual CPoint* GetJoint   (int index);


    CSpline();
    ~CSpline();
};

#endif // CSPLINE_H
