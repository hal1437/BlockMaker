#include "MakeObjectController.h"


CREATE_RESULT MakeObjectController::MakeJoint(CObject* obj,Pos pos,CObject* merge){

    CPoint* new_point;
    if(merge == nullptr){
        //新たに作成
        new_point = new CPoint(pos,obj);
    }else if(merge->is<CPoint>()){
        //mergeを点として使用
        new_point = dynamic_cast<CPoint*>(merge);
    }else{
        //mergeの近接点を作成
        new_point = new CPoint(merge->GetNearPos(pos));
    }
    //生成点を持つ
    this->last_point = new_point;

    //実行
    return obj->Create(new_point);
}


void MakeObjectController::StartMaking(MakeObject type,Pos pos,CObject* merge){
    if(type == MakeObject::POINT){    //点
        CPoint* pos = new CPoint(pos);
        //追加処理

    }else{//それ以外
        if(type == MakeObject::LINE  )this->making_object = new CLine  ();
        if(type == MakeObject::ARC   )this->making_object = new CArc   ();
        if(type == MakeObject::SPLINE)this->making_object = new CSpline();
        //startを作成する。
        this->making_step = MakeJoint(this->making_object,pos,merge);

        //持ち点を作成
        this->last_point = new CPoint(pos,this->making_object);
        this->making_object->Create(this->last_point);

        //モデルに追加
        this->model->AddEdges(this->making_object);
    }
}
void MakeObjectController::StepMaking (Pos pos,CObject* merge){

    //ジョイントを作成しつつ継続
    MakeJoint(this->making_object,pos,merge);
}
void MakeObjectController::EndMaking  (Pos pos,CObject* merge){
    if(merge != nullptr){
        if(merge->is<CPoint>()){
            //すり替え
            this->making_object->SetEndPos(dynamic_cast<CPoint*>(merge));
        }else{
            //持ち手の近似移動
            this->last_point->Move(merge->GetNearPos(pos) - *this->last_point);
        }
    }
    this->last_point = nullptr; //手放す
    this->making_object = nullptr;   //作成完了
}

void MakeObjectController::Making(MakeObject type,Pos pos,CObject* merge){
    if(this->making_step == COMPLETE){
        //開始
        StartMaking(type,pos,merge);
    }else{
        if(this->making_step != ENDLESS)this->making_step--;//作成過程を進める

        if(this->making_step == COMPLETE){
            //終了
            EndMaking(pos,merge);
        }else{
            //継続
            StepMaking(pos,merge);
        }

    }
}

void MakeObjectController::Escape(){
    //作成解除
    if(this->making_object != nullptr){
        if(this->making_step != ENDLESS){
            this->model->RemoveEdges(this->making_object);
        }
        this->making_object = nullptr;
        this->last_point    = nullptr;
        this->making_step   = COMPLETE;
    }
}


CPoint* MakeObjectController::GetLastPos(){
    return this->last_point;
}


MakeObjectController::MakeObjectController()
{

}

MakeObjectController::~MakeObjectController()
{

}

