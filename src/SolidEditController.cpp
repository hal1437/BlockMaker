#include "SolidEditController.h"

Quat SolidEditController::getConvertFrontToSide()const{
    return Quat::getRotateYMatrix(M_PI/2);
}
Quat SolidEditController::getConvertFrontToTop ()const{
    return Quat::getRotateXMatrix(M_PI/2);
}
Quat SolidEditController::getConvertSideToFront()const{
    return Quat::getRotateYMatrix(-M_PI/2);
}
Quat SolidEditController::getConvertTopToFront ()const{
    return Quat::getRotateXMatrix(-M_PI/2);
}
Quat SolidEditController::getConvertSideToTop  ()const{
    return this->getConvertSideToFront().Dot(this->getConvertFrontToTop());
}
Quat SolidEditController::getConvertTopToSide()const{
    return this->getConvertTopToFront().Dot(this->getConvertFrontToSide());
}

CFace* SolidEditController::getFrontFace_impl(Quat convert,Quat invert)const{
    //正面を軸として変換を通し各平面の大きさを取得する関数
    if(this->model->GetBlocks().empty()){
        CFace* face = new CFace();
        QVector<CPoint*> poses;
        poses.push_back(new CPoint(Pos( DEFAULT_FACE_LEGTH, DEFAULT_FACE_LEGTH,0).Dot(invert)));
        poses.push_back(new CPoint(Pos(-DEFAULT_FACE_LEGTH, DEFAULT_FACE_LEGTH,0).Dot(invert)));
        poses.push_back(new CPoint(Pos(-DEFAULT_FACE_LEGTH,-DEFAULT_FACE_LEGTH,0).Dot(invert)));
        poses.push_back(new CPoint(Pos( DEFAULT_FACE_LEGTH,-DEFAULT_FACE_LEGTH,0).Dot(invert)));
        face->edges.push_back(new CLine(poses[0],poses[1]));
        face->edges.push_back(new CLine(poses[1],poses[2]));
        face->edges.push_back(new CLine(poses[2],poses[3]));
        face->edges.push_back(new CLine(poses[3],poses[0]));
        face->SetPolygon(false);
        return face;
    }

    double top=0,bottom=0,right=0,left=0;
    for(CBlock* block:this->model->GetBlocks()){
        for(CFace* face:block->faces){
            for(CEdge* edge:face->edges){
                for(int j=0;j<edge->GetChildCount();j++){
                    right  = std::max(right ,((*edge->GetPoint(j)).Dot(convert)).mat[0]);
                    left   = std::min(left  ,((*edge->GetPoint(j)).Dot(convert)).mat[0]);
                    top    = std::max(top   ,((*edge->GetPoint(j)).Dot(convert)).mat[1]);
                    bottom = std::min(bottom,((*edge->GetPoint(j)).Dot(convert)).mat[1]);
                }
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

    CFace* face = new CFace();
    QVector<CPoint*> poses;
    poses.push_back(new CPoint(Pos( right ,top   ,0).Dot(invert)));
    poses.push_back(new CPoint(Pos( right ,bottom,0).Dot(invert)));
    poses.push_back(new CPoint(Pos( left  ,bottom,0).Dot(invert)));
    poses.push_back(new CPoint(Pos( left  ,top   ,0).Dot(invert)));
    face->edges.push_back(new CLine(poses[0],poses[1]));
    face->edges.push_back(new CLine(poses[1],poses[2]));
    face->edges.push_back(new CLine(poses[2],poses[3]));
    face->edges.push_back(new CLine(poses[3],poses[0]));
    face->SetPolygon(false);
    for(CPoint* pos:poses){
        pos->SetControlPoint(true);
    }
    return face;
}

CFace* SolidEditController::getFrontFace()const{//正面
    return this->getFrontFace_impl(Quat::getIdentityMatrix(),Quat::getIdentityMatrix());
}
CFace* SolidEditController::getTopFace  ()const{//平面
    return this->getFrontFace_impl(this->getConvertTopToFront(),this->getConvertFrontToTop());
}
CFace* SolidEditController::getSideFace ()const{//右側面
    return this->getFrontFace_impl(this->getConvertFrontToSide(),this->getConvertSideToFront());
}
Quat SolidEditController::getCameraMatrix()const{
    return Quat::getRotateXMatrix(theta1).Dot(Quat::getRotateYMatrix(theta2));
}

bool SolidEditController::isSketcheing()const{
    return (this->sketch_face != nullptr);
}


CObject* SolidEditController::getHangedObject(Pos center, Pos dir)const{
    CObject* ans = nullptr;

    //点の選択
    for(CPoint* p:this->model->GetPoints()){
        if(p->isVisible()){
            if((Pos::LineNearPoint(center,center+dir, *p) - *p).Length() < CPoint::COLLISION_SIZE && hang_point != p){
                //スケッチ中なら、平面上に存在する条件を追加
                if(this->isSketcheing()){
                    if(this->sketch_face->isComprehension(*p)){
                        ans = p;
                    }
                }else{
                    ans = p;
                }
            }
        }
    }
    if(ans == nullptr){
        //エッジの選択
        for(CEdge* e : this->model->GetEdges()){
            if(hang_point == e->start || hang_point == e->end || !e->isVisible())continue;
            //近似点選択
            const double DIVIDE = 100;
            for(int i=0;i<=DIVIDE;i++){
                Pos p = e->GetMiddleDivide(i/DIVIDE);
                if((Pos::LineNearPoint(center,center+dir, p) - p).Length() < CPoint::COLLISION_SIZE){
                    //スケッチ中なら、平面上に存在する条件を追加
                    if(this->isSketcheing()){
                        if(this->sketch_face->isComprehension(*e->start) && this->sketch_face->isComprehension(*e->end)){
                            ans = e;
                        }
                    }else{
                        ans = e;
                    }
                }
            }
        }
    }
    return ans;
}

CFace* SolidEditController::getHangedFace(Pos center,Pos camera_pos)const{
    if(this->getHangedObject(center,camera_pos-center) != nullptr) return nullptr;

    //最も近い面を選択する
    QVector<std::pair<double,CFace*>> rank;
    rank.push_back(std::make_pair(Collision::GetLengthFaceToLine(CFace::base[0] ,camera_pos,camera_pos-center),CFace::base[0]));
    rank.push_back(std::make_pair(Collision::GetLengthFaceToLine(CFace::base[1] ,camera_pos,camera_pos-center),CFace::base[1]));
    rank.push_back(std::make_pair(Collision::GetLengthFaceToLine(CFace::base[2] ,camera_pos,camera_pos-center),CFace::base[2]));
    for(CFace* f:this->model->GetFaces()){
        if(f->isVisible()){
            rank.push_back(std::make_pair(Collision::GetLengthFaceToLine(f,camera_pos,camera_pos-center),f));
        }
    }

    if(std::all_of(rank.begin(),rank.end(),[](std::pair<double,CFace*> v){return v.first==-1;})){
        return nullptr;
    }else{
        rank.erase(std::remove_if(rank.begin(),rank.end(),[](std::pair<double,CFace*> v){
            return v.first==-1;
        }),rank.end());
        std::pair<double,CFace*> a = *std::min_element(rank.begin(),rank.end(),[](std::pair<double,CFace*> lhs,std::pair<double,CFace*> rhs){
            return lhs.first < rhs.first;
        });
        //qDebug() << a.first;
        return a.second;
    }

}

SolidEditController::SolidEditController(QObject *parent):
    QObject(parent)
{
}

SolidEditController::~SolidEditController()
{
}

void SolidEditController::Create3Face(){
    for(int i=0;i<3;i++){
        if(CFace::base[i] != nullptr) delete CFace::base[i];
    }
    CFace::base[0] = this->getFrontFace();
    CFace::base[1] = this->getTopFace();
    CFace::base[2] = this->getSideFace();
    CFace::base[0]->setName("正面");
    CFace::base[1]->setName("平面");
    CFace::base[2]->setName("右側面");
    for(int i=0;i<3;i++){
        this->model->AddFaces(CFace::base[i]);
    }
}
void SolidEditController::CreateOrigin(){
    if(CPoint::origin != nullptr) delete CPoint::origin;
    CPoint::origin = new CPoint(nullptr);
    CPoint::origin->setName("原点");
    CPoint::origin->SetControlPoint(true);
    this->model->AddPoints(CPoint::origin);
}

