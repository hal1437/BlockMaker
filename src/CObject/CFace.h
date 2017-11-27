#ifndef CFACE_H
#define CFACE_H

#include "CEdge.h"
#include "Collision.h"

//境界面クラス
class Boundary{
public:
    //境界の種類
    enum class Type{
        empty         = 0,
        patch         = 1,
        wall          = 2,
        symmetryPlane = 3,
        cyclic        = 4,
        cyclicAMI     = 5,
        wedge         = 6,
        undefined     = 7,//連続
    };

    //文字列からBoundary::Typeに変換
    static Type    StringToBoundaryType(QString str);
    static QString BoundaryTypeToString(Boundary::Type type);
public:
    QString name;
    Type type;
    bool operator==(Boundary rhs)const{return (this->name == rhs.name && this->type == rhs.type);}
    bool operator!=(Boundary rhs)const{return !(*this == rhs);}
};

//平面オブジェクト
class CFace : public CObject
{
    Q_OBJECT
public:
    static CFace* base[3]; //正面,平面,右側面

    QVector<CEdge*> edges;                  // 構成線
    OBSERVE_MEMBER(Boundary,Boundary,boundary) // 境界タイプ
    DEFINE_FLAG(VisibleDetail,true)         // 分割フレーム表示
    DEFINE_FLAG(BaseFace ,true)               // ポリゴン判定
    DEFINE_FLAG(FaceBlend,true)             // 面透過判定

    QVector<int> reorder;//エッジ反転係数
    QVector<QVector<Pos>> mesh_memory;//メッシュ分割位置記録二次元配列
public:
    //面が作成可能か
    static  bool Creatable(QVector<CObject*> lines);
    virtual void Create(QVector<CEdge*> edges);     //作成

    virtual void DrawGL(Pos camera,Pos center)const; //三次元描画関数
    virtual bool DrawNormArrowGL()const;             //三次元法線ベクトル描画関数
    virtual void DrawFillGL()const;                  //三次元面描画関数
    virtual void DrawMeshGL()const;                  //三次元メッシュ関数

    virtual bool isComprehension(Pos pos)const; //平面上かチェックする。
    virtual Pos  GetNorm()const;               //法線ベクトル取得
    virtual Pos  GetNorm(double u,double v)const;               //法線ベクトル取得

    virtual void ReorderEdges(); //エッジ方向係数再計算
    virtual void RecalcMesh();   //メッシュ位置再計算

    //UV座標取得
    virtual Pos  GetPosFromUV      (double u,double v)const; //UV座標取得
    virtual Pos  GetPosFromUVSquare(double u,double v)const; //UV座標取得(全て直線と仮定して)

    //精密取得関数
    virtual CPoint* GetBasePoint()const;                //基準点取得
    virtual CEdge*  GetBaseEdge ()const;                //基準線取得
    virtual CPoint* GetPointSequence(int index)const;   //番号順点取得
    virtual CEdge*  GetEdgeSequence (int index)const;   //番号順線取得

    //子の操作
    virtual CEdge*   GetEdge (int index);
    virtual CEdge*   GetEdge (int index)const;
    virtual CObject* GetChild(int index);
    virtual void     SetChild(int index,CObject* obj);
    virtual int      GetChildCount()const;
    virtual Pos      GetEdgeMiddle(int index,double t)const; //reorderに依存する中間点取得
    virtual double   GetGrading   (int index)const;          //reorderに依存するエッジ寄せ係数取得

    //近接点
    virtual Pos GetNearPos (const Pos& hand)const;
    virtual Pos GetNearLine(const Pos& pos1,const Pos& pos2)const;

    //複製
    virtual CObject* Clone()const;

    CFace(QObject* parent=nullptr);
    ~CFace();

public slots:
    //線移動コールバック
    virtual void ChangeChildCallback(QVector<CObject*> egde);
public:

    //線分との距離取得
    double GetLengthFaceToLine(Pos camera,Pos dir);
    //線分との衝突判定
    bool   CheckHitFaceToLine (Pos camera,Pos dir);
};

#endif // CFACE_H
