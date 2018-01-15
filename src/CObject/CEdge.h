#ifndef CEDGE_H
#define CEDGE_H
#include "CPoint.h"


//エッジクラス
class CEdge :public CObject
{
    Q_OBJECT

public:
    const static int COLLISION_SIZE   = 5; //当たり判定半径

    //エッジ寄せ係数
    struct Grading{
        struct GradingElement{
            double dir;     //方向割合
            double cell;    //分割数割合
            double grading; //寄せ係数
            bool operator==(GradingElement rhs)const{return std::tie(this->dir,this->cell,this->grading) == std::tie(rhs.dir,rhs.cell,rhs.grading); }
        };
        QList<GradingElement> elements;
        Grading GetReverse()const;//反転したものを取得
        bool operator==(Grading rhs)const{return this->elements == rhs.elements; }
        GradingElement& operator[](int index){return this->elements[index]; }
    };


public:
    CPoint* start; //エッジの始点
    CPoint* end;   //エッジの終点
    OBSERVE_MEMBER(Grading,Grading,grading)//エッジ寄せ係数
    OBSERVE_MEMBER(int    ,Divide ,divide)//分割数
public:
    //分割レート取得
    static double GetDivisionRate(int divide,Grading grading, int count);

public:
    virtual QString DefaultClassName(){return "CEdge";}

    virtual CREATE_RESULT Create(CPoint* pos) = 0;   //作成関数
    virtual void DrawArrow(double start,double end)const; //三次元描画関数
    virtual void DrawGL(Pos camera,Pos center)const; //三次元描画関数

    //中間点操作
    virtual CPoint*  GetPoint(int index) = 0;
    virtual CObject* GetChild(int index) = 0;
    virtual void     SetChild(int index,CObject* obj) = 0;
    virtual int      GetChildCount()const = 0;
    virtual Pos      GetMiddleDivide(double t)const = 0;//補間点取得
    virtual Pos      GetDivisionPoint(int count)const; //エッジ点取得
    virtual void     SetStartPos(CObject* obj);
    virtual void     SetEndPos(CObject* obj);

    //近接点
    virtual bool isOnEdge   (const Pos& hand)const;
    virtual Pos  GetNearPos (const Pos& hand)const;
    virtual Pos  GetNearLine(const Pos& pos1,const Pos& pos2)const;

    //複製
    virtual CObject* Clone()const = 0;

    //探索系
    Pos    GetDifferentialVec   (Pos pos)const; //posの位置で微分した基底ベクトル取得
    double GetMiddleParamFromPos(Pos pos)const; //tを求める

    //結合
    virtual CEdge*          MergeEdge (CEdge* merge_point); //失敗ならnullptr
    virtual QList<CEdge*> DivideEdge(CPoint* division);   //失敗ならQList<CEdge*>()

    CEdge(QObject* parent=nullptr);
    CEdge(CPoint* start,CPoint* end,QObject* parent=nullptr);
    ~CEdge();

public slots:
    //点移動コールバック
    virtual void ChangeChildCallback(QList<CObject*> child);

};

#endif // CEDGE_H
