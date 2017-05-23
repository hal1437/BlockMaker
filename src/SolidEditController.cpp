#include "SolidEditController.h"

Matrix<double, 3, 3> SolidEditController::getConvertFrontToSide()const{
    return Matrix<double, 3, 3>({ 0.0f,0.0f,1.0f,
                                  0.0f,1.0f,0.0f,
                                 -1.0f,0.0f,0.0f});
}
Matrix<double, 3, 3> SolidEditController::getConvertFrontToTop ()const{
    return Matrix<double, 3, 3>({1.0f,0.0f,0.0f,
                                 0.0f,0.0f,-1.0f,
                                 0.0f,1.0f,0.0f});
}
Matrix<double, 3, 3> SolidEditController::getConvertSideToFront()const{
    return Matrix<double, 3, 3>({0.0f,0.0f,-1.0f,
                                 0.0f,1.0f, 0.0f,
                                 1.0f,0.0f, 0.0f});
}
Matrix<double, 3, 3> SolidEditController::getConvertTopToFront ()const{
    return Matrix<double, 3, 3>({1.0f, 0.0f,0.0f,
                                 0.0f, 0.0f,1.0f,
                                 0.0f,-1.0f,0.0f});
}
Matrix<double, 3, 3> SolidEditController::getConvertSideToTop  ()const{
    return this->getConvertSideToFront().Dot(this->getConvertFrontToTop());
}
Matrix<double, 3, 3> SolidEditController::getConvertTopToSide()const{
    return this->getConvertTopToFront().Dot(this->getConvertFrontToSide());
}

Face SolidEditController::getFrontFace_impl(Matrix<double, 3, 3> convert,Matrix<double, 3, 3> invert)const{
    //正面を軸として変換を通し各平面の大きさを取得する関数
    if(this->model->GetBlocks().empty())return Face{{Pos( DEFAULT_FACE_LEGTH, DEFAULT_FACE_LEGTH,0).Dot(invert),
                                                     Pos(-DEFAULT_FACE_LEGTH, DEFAULT_FACE_LEGTH,0).Dot(invert),
                                                     Pos(-DEFAULT_FACE_LEGTH,-DEFAULT_FACE_LEGTH,0).Dot(invert),
                                                     Pos( DEFAULT_FACE_LEGTH,-DEFAULT_FACE_LEGTH,0).Dot(invert)}};
    double top=0,bottom=0,right=0,left=0;
    for(CBlock* block:this->model->GetBlocks()){
        for(int i=0;i<4;i++){
            for(int j=0;j<block->GetEdge(i)->GetPosSequenceCount();j++){
                top    = std::max(top   ,((*block->GetEdge(i)->GetPosSequence(j)).Dot(invert)).mat[1]);
                bottom = std::min(bottom,((*block->GetEdge(i)->GetPosSequence(j)).Dot(invert)).mat[1]);
                right  = std::max(right ,((*block->GetEdge(i)->GetPosSequence(j)).Dot(invert)).mat[0]);
                left   = std::min(left  ,((*block->GetEdge(i)->GetPosSequence(j)).Dot(invert)).mat[0]);
            }
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
    bottom -= height_delta*0.1;
    right  += widht_delta *0.1;
    left   -= widht_delta *0.1;

    return Face{{Pos( right ,top   ,0).Dot(invert),
                 Pos( right ,bottom,0).Dot(invert),
                 Pos( left  ,bottom,0).Dot(invert),
                 Pos( left  ,top   ,0).Dot(invert)}};
}

Face SolidEditController::getFrontFace()const{//正面
    return this->getFrontFace_impl(Matrix<double,3,3>::getIdentityMatrix(),Matrix<double,3,3>::getIdentityMatrix());
}
Face SolidEditController::getTopFace  ()const{//平面
    return this->getFrontFace_impl(this->getConvertFrontToTop(),this->getConvertTopToFront());
}
Face SolidEditController::getSideFace ()const{//右側面
    return this->getFrontFace_impl(this->getConvertFrontToSide(),this->getConvertSideToFront());
}

Face SolidEditController::getHangedFace(Pos center, Pos dir)const{
    if(Collision::CheckHitFaceToLine(this->getFrontFace(),Line{center,center+dir}))return this->getFrontFace();
    if(Collision::CheckHitFaceToLine(this->getTopFace()  ,Line{center,center+dir}))return this->getTopFace();
    if(Collision::CheckHitFaceToLine(this->getSideFace() ,Line{center,center+dir}))return this->getSideFace();
    return Face();
}

SolidEditController::SolidEditController(QObject *parent):
    QObject(parent)
{

}

SolidEditController::~SolidEditController()
{
}

