#include "SolidEditController.h"

Matrix<double, 3, 3> SolidEditController::getConvertFrontToSide()const{
    return Matrix<double, 3, 3>({0.0f,0.0f,0.0f,
                                 0.0f,0.0f,0.0f,
                                 0.0f,0.0f,0.0f});
}
Matrix<double, 3, 3> SolidEditController::getConvertFrontToTop ()const{
    return Matrix<double, 3, 3>({0.0f,0.0f,0.0f,
                                 0.0f,0.0f,0.0f,
                                 0.0f,0.0f,0.0f});
}
Matrix<double, 3, 3> SolidEditController::getConvertSideToFront()const{
    return Matrix<double, 3, 3>({0.0f,0.0f,0.0f,
                                 0.0f,0.0f,0.0f,
                                 0.0f,0.0f,0.0f});
}
Matrix<double, 3, 3> SolidEditController::getConvertTopToFront ()const{
    return Matrix<double, 3, 3>({0.0f,0.0f,0.0f,
                                 0.0f,0.0f,0.0f,
                                 0.0f,0.0f,0.0f});
}
Matrix<double, 3, 3> SolidEditController::getConvertSideToTop  ()const{
    return Matrix<double, 3, 3>({0.0f,0.0f,0.0f,
                                 0.0f,0.0f,0.0f,
                                 0.0f,0.0f,0.0f});
}
Matrix<double, 3, 3> SolidEditController::getConvertTopToSide()const{
    return Matrix<double, 3, 3>({0.0f,0.0f,0.0f,
                                 0.0f,0.0f,0.0f,
                                 0.0f,0.0f,0.0f});
}



Face SolidEditController::getFrontFace_impl(Matrix<double, 3, 3> convert,Matrix<double, 3, 3> invert)const{//正面
    if(this->model->GetEdges().empty())return Face{{Pos( DEFAULT_FACE_LEGTH, DEFAULT_FACE_LEGTH,0),
                                                    Pos(-DEFAULT_FACE_LEGTH, DEFAULT_FACE_LEGTH,0),
                                                    Pos(-DEFAULT_FACE_LEGTH,-DEFAULT_FACE_LEGTH,0),
                                                    Pos( DEFAULT_FACE_LEGTH,-DEFAULT_FACE_LEGTH,0)}};
    double top,bottom,right,left;
    for(CEdge* edge:this->model->GetEdges()){
        for(int i=0;i<edge->GetPosSequenceCount();i++){
            top    = std::max(top   ,((*edge->GetPosSequence(i)).Dot(convert)).mat[1]);
            bottom = std::min(bottom,((*edge->GetPosSequence(i)).Dot(convert)).mat[1]);
            right  = std::max(right ,((*edge->GetPosSequence(i)).Dot(convert)).mat[0]);
            left   = std::min(left  ,((*edge->GetPosSequence(i)).Dot(convert)).mat[0]);
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

    return Face{{Pos( top   ,right,0).Dot(invert),
                 Pos( bottom,right,0).Dot(invert),
                 Pos( bottom,left ,0).Dot(invert),
                 Pos( top   ,left ,0).Dot(invert)}};
}


Face SolidEditController::getFrontFace()const{//正面
    Matrix<double,3,3>matrix = Matrix<double,3,3>::getIdentityMatrix();
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

