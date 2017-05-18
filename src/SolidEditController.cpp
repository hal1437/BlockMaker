#include "SolidEditController.h"



Face SolidEditController::getFrontFace()const{//正面
    if(this->model->GetEdges().empty())return Face{{Pos( DEFAULT_FACE_LEGTH, DEFAULT_FACE_LEGTH,0),
                                                    Pos(-DEFAULT_FACE_LEGTH, DEFAULT_FACE_LEGTH,0),
                                                    Pos(-DEFAULT_FACE_LEGTH,-DEFAULT_FACE_LEGTH,0),
                                                    Pos( DEFAULT_FACE_LEGTH,-DEFAULT_FACE_LEGTH,0)}};
    double top,bottom,right,left;
    for(CEdge* edge:this->model->GetEdges()){
        for(int i=0;i<edge->GetPosSequenceCount();i++){
            top    = std::max(top   ,edge->GetPosSequence(i)->y);
            bottom = std::min(bottom,edge->GetPosSequence(i)->y);
            right  = std::max(right ,edge->GetPosSequence(i)->x);
            left   = std::min(left  ,edge->GetPosSequence(i)->x);
        }
    }
    double height_delta = top - bottom;
    double widht_delta  = right  - left;

    //0補正
    if(height_delta == 0 && widht_delta  == 0){
        top = bottom = right = left =   DEFAULT_FACE_LEGTH;
        height_delta = widht_delta  = 2*DEFAULT_FACE_LEGTH;
    }
    if(height_delta == 0)height_delta = widht_delta;
    if(widht_delta  == 0)widht_delta = height_delta;

    //幅の10%を足す
    top    += height_delta*0.1;
    bottom += height_delta*0.1;
    right  += widht_delta *0.1;
    left   += widht_delta *0.1;

    return Face{{Pos( top   ,right,0),
                 Pos( bottom,right,0),
                 Pos( bottom,left ,0),
                 Pos( top   ,left ,0)}};
}
Face SolidEditController::getTopFace  ()const{//平面
}
Face SolidEditController::getSideFace ()const{//右側面

}

SolidEditController::SolidEditController(QObject *parent):
    QObject(parent)
{

}

SolidEditController::~SolidEditController()
{
}

