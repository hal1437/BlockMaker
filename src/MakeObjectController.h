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
class MakeObjectController
{
    //生成方法
    //生成タイプと座標をを選択してStartMaking();

    int making_step; //生成段階
    Quat converter;  //変換行列

public:
    static CObject* GetNearPos(Pos pos);           //近接点を取得(点)
    static CObject* GetNearPos(Pos pos1,Pos pos2); //近接点を取得(線)

public:

    void SetConvertMatrix(Quat);//変換

    Pos ConvertMatrix();//変換


    void StartMaking(MakeObject type,Pos pos,CObject* obj = nullptr); //生成開始
    void StepMaking (Pos pos,CObject* obj = nullptr);                 //追加生成
    void EndMaking  (Pos pos,CObject* obj = nullptr);                 //生成終了

    MakeObjectController();
    ~MakeObjectController();
};

#endif // MAKEOBJECTCONTROLLER_H
