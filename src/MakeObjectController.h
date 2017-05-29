#ifndef MAKEOBJECTCONTROLLER_H
#define MAKEOBJECTCONTROLLER_H
#include "CadModelCore.h"

enum MakeObject{
    POINT ,
    LINE  ,
    ARC   ,
    SPLINE,
};

//オブジェクト生成に関するコントローラ
class MakeObjectController : public CadModelCoreInterface
{
private:
    CPoint*  last_point;    //最終生成点
    CEdge* making_object; //生成オブジェクト
    int      making_step;   //生成段階

private:

    CREATE_RESULT MakeJoint(CObject* obj,Pos pos, CObject *merge);
    void StartMaking(MakeObject type,Pos pos,CObject* merge = nullptr); //生成開始
    void StepMaking (Pos pos,CObject* merge = nullptr);                 //追加生成
    void EndMaking  (Pos pos,CObject* merge = nullptr);                 //生成終了

public:

    //作成
    void Making(MakeObject type,Pos pos,CObject* merge = nullptr);


    //生成中断
    void Escape();

    //最終点を保持
    CPoint* GetLastPos();

    MakeObjectController();
    ~MakeObjectController();
};

#endif // MAKEOBJECTCONTROLLER_H
