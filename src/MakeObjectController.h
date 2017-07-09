#ifndef MAKEOBJECTCONTROLLER_H
#define MAKEOBJECTCONTROLLER_H
#include "CadModelCore.h"

enum class MAKE_OBJECT{
    Edit  ,//なし
    Point ,
    Line  ,
    Arc   ,
    Spline,
    Rect
};

//オブジェクト生成に関するコントローラ
class MakeObjectController : public CadModelCoreInterface
{
private:
    CPoint*  last_point    = nullptr;  //最終生成点
    CEdge*   making_object = nullptr;  //生成オブジェクト
    int      making_step   = COMPLETE; //生成段階
    int      making_count  = 0;        //生成番号

private:

    CREATE_RESULT MakeJoint(CEdge* obj,Pos pos, CObject *merge);
    void StartMaking(MAKE_OBJECT type, Pos pos, CObject* merge = nullptr); //生成開始
    void StepMaking (Pos pos,CObject* merge = nullptr);                 //追加生成
    void EndMaking  (Pos pos,CObject* merge = nullptr);                 //生成終了

public:

    //作成
    void Making(MAKE_OBJECT type,Pos pos,CObject* merge = nullptr);


    //生成中断
    void Escape();

    //最終点を保持
    CPoint* GetLastPos();

    MakeObjectController();
    virtual ~MakeObjectController();
};

#endif // MAKEOBJECTCONTROLLER_H
