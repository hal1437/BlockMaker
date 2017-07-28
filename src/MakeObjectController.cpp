#include "MakeObjectController.h"


CREATE_RESULT MakeObjectController::MakeJoint(CEdge* obj,Pos pos,CObject* merge){

    CPoint* new_point;
    if(merge == nullptr){
        //新たに作成
        new_point = new CPoint(pos,obj);
        //生成点を持つ
        this->last_point = new_point;
    }else if(merge->is<CPoint>()){
        //持ち手を破棄
        this->model->RemovePoints(this->last_point);
        //mergeを点として使用
        new_point = new CPoint(pos,obj);
        //すり替え
        if(this->making_step == ENDLESS){
            //終了
            if(obj->GetChildCount() > 0 && obj->GetChild(obj->GetChildCount()-1) == merge){
                obj->SetEndPos(obj->GetChild(obj->GetChildCount()-1));
                obj->SetChild(obj->GetChildCount()-1,nullptr);
                this->last_point=nullptr;
                this->making_count = -1;//手放しまで完了
                return COMPLETE;
            }else{
                //追加
                //obj->SetChild(dynamic_cast<CPoint*>(merge),obj->GetPosSequenceCount()-2);
                obj->SetChild(this->making_count,dynamic_cast<CPoint*>(merge));
            }
        }else{
            //円弧は例外
            if(obj->is<CArc>() && this->making_count <= 1){
                if(this->making_count == 0)obj->SetChild(0,dynamic_cast<CPoint*>(merge));
                if(this->making_count == 1)obj->SetStartPos(dynamic_cast<CPoint*>(merge));
            }else{
                obj->SetChild(this->making_count,dynamic_cast<CPoint*>(merge));
            }
        }
        this->last_point = new_point;
    }else{
        //mergeの近接点を作成
        new_point = new CPoint(merge->GetNearPos(pos));
    }
    this->model->AddPoints(new_point);

    //実行
    return obj->Create(new_point);
}


void MakeObjectController::StartMaking(MAKE_OBJECT type,Pos pos,CObject* merge){
    if(type == MAKE_OBJECT::Point){    //点
        CPoint* new_pos = new CPoint(pos);
        //追加処理
        this->model->AddPoints(new_pos);

    }else{//それ以外
        if(type == MAKE_OBJECT::Line  )this->making_object = new CLine  ();
        if(type == MAKE_OBJECT::Arc   )this->making_object = new CArc   ();
        if(type == MAKE_OBJECT::Spline)this->making_object = new CSpline();

        //持ち点を作成
        this->last_point = new CPoint(pos,this->making_object);
        this->making_object->Create(this->last_point);
        this->model->AddPoints(this->last_point);

        //startを作成する。
        this->making_step = MakeJoint(this->making_object,pos,merge);


        //モデルに追加
        this->model->AddPoints(this->last_point);
        this->model->AddEdges(this->making_object);
        this->making_count++;
    }
}
void MakeObjectController::StepMaking (Pos pos,CObject* merge){
    //ジョイントを作成しつつ継続
    this->making_step = MakeJoint(this->making_object,pos,merge);
    this->making_count++;
}
void MakeObjectController::EndMaking  (Pos pos,CObject* merge){
    if(merge != nullptr){
        if(merge->is<CPoint>()){
            //すり替え
            this->making_object->SetEndPos(dynamic_cast<CPoint*>(merge));
            //保持点をモデルから削除
            this->model->RemovePoints(this->last_point);
        }else{
            //持ち手の近似移動
            this->last_point->MoveAbsolute(merge->GetNearPos(pos));
        }
    }
    this->last_point    = nullptr; //手放す
    this->making_object = nullptr;   //作成完了
    this->making_count  = 0;
}
void MakeObjectController::Making(MAKE_OBJECT type, Pos pos, CObject* merge){
    if(this->making_step == COMPLETE){
        if(this->making_count == 0){
            //新規開始
            StartMaking(type,pos,merge);
        } else{
            //手放し処理
            EndMaking(pos,merge);
        }
    }else{
        //継続
        StepMaking(pos,merge);
    }
    this->model->UpdateObject();
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

