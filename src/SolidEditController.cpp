#include "SolidEditController.h"

Quat SolidEditController::getConvertFrontToSide()const{
    return Quat::getRotateYMatrix(M_PI/2);
}
Quat SolidEditController::getConvertFrontToTop ()const{
    return Quat::getRotateXMatrix(-M_PI/2);
}
Quat SolidEditController::getConvertSideToFront()const{
    return Quat::getRotateYMatrix(-M_PI/2);
}
Quat SolidEditController::getConvertTopToFront ()const{
    return Quat::getRotateXMatrix(M_PI/2);
}
Quat SolidEditController::getConvertSideToTop  ()const{
    return this->getConvertSideToFront().Dot(this->getConvertFrontToTop());
}
Quat SolidEditController::getConvertTopToSide()const{
    return this->getConvertTopToFront().Dot(this->getConvertFrontToSide());
}

Face SolidEditController::getFrontFace_impl(Quat convert,Quat invert)const{
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
    if(NearlyEqual(height_delta,0) && NearlyEqual(widht_delta,0)){
        top = bottom = right = left =   DEFAULT_FACE_LEGTH;
        height_delta = widht_delta  = 2*DEFAULT_FACE_LEGTH;
    }
    if(NearlyEqual(height_delta,0))height_delta = widht_delta;
    if(NearlyEqual(widht_delta,0))widht_delta = height_delta;

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
    return this->getFrontFace_impl(Quat::getIdentityMatrix(),Quat::getIdentityMatrix());
}
Face SolidEditController::getTopFace  ()const{//平面
    return this->getFrontFace_impl(this->getConvertFrontToTop(),this->getConvertTopToFront());
}
Face SolidEditController::getSideFace ()const{//右側面
    return this->getFrontFace_impl(this->getConvertFrontToSide(),this->getConvertSideToFront());
}


CObject* SolidEditController::getHangedObject(Pos center, Pos dir)const{
    Line line = Line{center,center+dir};
    CObject* ans = nullptr;

    //点の選択
    for(int i=0;i<this->model->GetEdges().size();i++){
        CEdge* e = this->model->GetEdges()[i];
        if((Pos::LineNearPoint(line.pos1,line.pos2, *e->start) - *e->start).Length() < CPoint::COLLISION_SIZE)ans = e->start;
        if((Pos::LineNearPoint(line.pos1,line.pos2, *e->end  ) - *e->end  ).Length() < CPoint::COLLISION_SIZE)ans = e->end;
    }
    return ans;
}

Face SolidEditController::getHangedFace(Pos center, Pos dir)const{

    //最も近い面を選択する
    QVector<std::pair<double,Face>> rank;
    rank.push_back(std::make_pair(Collision::GetLengthFaceToLine(this->getFrontFace(),Line{center,center+dir}),this->getFrontFace()));
    rank.push_back(std::make_pair(Collision::GetLengthFaceToLine(this->getTopFace()  ,Line{center,center+dir}),this->getTopFace()));
    rank.push_back(std::make_pair(Collision::GetLengthFaceToLine(this->getSideFace() ,Line{center,center+dir}),this->getSideFace()));

    if(std::all_of(rank.begin(),rank.end(),[](std::pair<double,Face> v){return v.first==-1;})){
        return Face();
    }else{
        return std::min_element(rank.begin(),rank.end(),[](std::pair<double,Face> lhs,std::pair<double,Face> rhs){
            return lhs.first > rhs.first;
        })->second;
    }

}

SolidEditController::SolidEditController(QObject *parent):
    QObject(parent)
{

}

SolidEditController::~SolidEditController()
{
}

