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
    CFace* face = new CFace();
    face->SetPolygon(false);
    face->SetVisibleDetail(false);

    //何もない時
    if(this->model->GetPoints().empty()){
        QVector<CPoint*> poses;
        poses.push_back(new CPoint(Pos( DEFAULT_FACE_LEGTH, DEFAULT_FACE_LEGTH,0).Dot(invert)));
        poses.push_back(new CPoint(Pos(-DEFAULT_FACE_LEGTH, DEFAULT_FACE_LEGTH,0).Dot(invert)));
        poses.push_back(new CPoint(Pos(-DEFAULT_FACE_LEGTH,-DEFAULT_FACE_LEGTH,0).Dot(invert)));
        poses.push_back(new CPoint(Pos( DEFAULT_FACE_LEGTH,-DEFAULT_FACE_LEGTH,0).Dot(invert)));
        face->edges.push_back(new CLine(poses[0],poses[1]));
        face->edges.push_back(new CLine(poses[1],poses[2]));
        face->edges.push_back(new CLine(poses[2],poses[3]));
        face->edges.push_back(new CLine(poses[3],poses[0]));
        return face;
    }

    double top=0,bottom=0,right=0,left=0;
    for(CPoint* pos:this->model->GetPoints()){
        right  = std::max(right ,(pos->Dot(convert)).mat[0]);
        left   = std::min(left  ,(pos->Dot(convert)).mat[0]);
        top    = std::max(top   ,(pos->Dot(convert)).mat[1]);
        bottom = std::min(bottom,(pos->Dot(convert)).mat[1]);
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

    QVector<CPoint*> poses;
    poses.push_back(new CPoint(Pos( right ,top   ,0).Dot(invert)));
    poses.push_back(new CPoint(Pos( right ,bottom,0).Dot(invert)));
    poses.push_back(new CPoint(Pos( left  ,bottom,0).Dot(invert)));
    poses.push_back(new CPoint(Pos( left  ,top   ,0).Dot(invert)));
    face->edges.push_back(new CLine(poses[0],poses[1]));
    face->edges.push_back(new CLine(poses[1],poses[2]));
    face->edges.push_back(new CLine(poses[2],poses[3]));
    face->edges.push_back(new CLine(poses[3],poses[0]));
    for(CPoint* pos:poses){
        pos->SetControlPoint(true);
    }
    return face;
}

CFace* SolidEditController::getFrontFace()const{//正面
    CFace* face = this->getFrontFace_impl(Quat::getIdentityMatrix(),Quat::getIdentityMatrix());
    face->setName("正面");
    return face;
}
CFace* SolidEditController::getTopFace  ()const{//平面
    CFace* face = this->getFrontFace_impl(this->getConvertTopToFront(),this->getConvertFrontToTop());
    face->setName("平面");
    return face;
}
CFace* SolidEditController::getSideFace ()const{//右側面
    CFace* face = this->getFrontFace_impl(this->getConvertFrontToSide(),this->getConvertSideToFront());
    face->setName("右側面");
    return face;
}
Quat SolidEditController::getCameraMatrix()const{
    return Quat::getRotateXMatrix(theta1).Dot(Quat::getRotateYMatrix(theta2));
}

bool SolidEditController::isSketcheing()const{
    return (projection_norm != Pos());
}


CObject* SolidEditController::getHangedObject(Pos center, Pos dir,double zoom_rate)const{
    QVector<QPair<CObject*,double>>ans;

    //点の選択
    for(CPoint* p:this->model->GetPoints()){
        if(p->isVisible()){
            double length = (Pos::LineNearPoint(center,center+dir, *p) - *p).Length() / zoom_rate;
            if(length < CPoint::COLLISION_SIZE && hang_point != p){
                //スケッチ中なら、平面上に存在する条件を追加
                if(this->isSketcheing()){
                    if(Collision::ChackPointOnFace(projection_norm,projection_center,*p)){
                        ans.push_back(qMakePair(p,length));
                    }
                }else{
                    ans.push_back(qMakePair(p,length));
                }
            }
        }
    }
    if(ans.size() == 0){
        //エッジの選択
        for(CEdge* e : this->model->GetEdges()){
            if(hang_point == e->start || hang_point == e->end || !e->isVisible())continue;
            //近似点選択
            const double DIVIDE = 100;
            for(int i=0;i<=DIVIDE;i++){
                Pos p = e->GetMiddleDivide(i/DIVIDE);
                double length = (Pos::LineNearPoint(center,center+dir, p) - p).Length() / zoom_rate;
                if(length  < CPoint::COLLISION_SIZE){
                    //スケッチ中なら、平面上に存在する条件を追加
                    if(this->isSketcheing()){
                        if(Collision::ChackPointOnFace(projection_norm,projection_center,*e->start) &&
                           Collision::ChackPointOnFace(projection_norm,projection_center,*e->end)){
                            ans.push_back(qMakePair(e,length));
                        }
                    }else{
                        ans.push_back(qMakePair(e,length));
                    }
                }
            }
        }
    }
    if(ans.size() == 0)return nullptr;
    else{
        return std::min_element(ans.begin(),ans.end(),[](QPair<CObject*,double> lhs,QPair<CObject*,double> rhs){
            return (lhs.second < rhs.second);
        })->first;
    }
}

CFace* SolidEditController::getHangedFace(Pos center,Pos camera_pos,double zoom_rate)const{
    if(this->getHangedObject(center,camera_pos-center,zoom_rate) != nullptr) return nullptr;

    //最も近い面を選択する
    QVector<std::pair<double,CFace*>> rank;
    for(CFace* f:this->model->GetFaces()){
        if(f->isVisible() && f->CheckHitFaceToLine(camera_pos,center-camera_pos)){
            rank.push_back(std::make_pair(f->GetLengthFaceToLine(camera_pos,center-camera_pos),f));
        }
    }
    //非接触面は削除
    rank.erase(std::remove_if(rank.begin(),rank.end(),[](std::pair<double,CFace*> v){
        return v.first==-1;
    }),rank.end());

    if(rank.size()==0) return nullptr;

    //近い順に並び替え
    std::pair<double,CFace*> a = *std::min_element(rank.begin(),rank.end(),[](std::pair<double,CFace*> lhs,std::pair<double,CFace*> rhs){
        return lhs.first < rhs.first;
    });
    return a.second;
}

SolidEditController::SolidEditController(QObject *parent):
    QObject(parent)
{
}

SolidEditController::~SolidEditController()
{
}

void SolidEditController::Create3Face(){
    if(exist(CFace::base,nullptr)){
        //新規作成
        CFace::base[0] = this->getFrontFace();
        CFace::base[1] = this->getTopFace();
        CFace::base[2] = this->getSideFace();
        for(int i =0;i<3;i++){
            this->model->AddFaces(CFace::base[i]);
        }
    }else{
        //引き継ぎ
        CFace* refresh[3] ={this->getFrontFace(),this->getTopFace(),this->getSideFace()};
        for(int i =0;i<3;i++){
            CFace::base[i]->edges = refresh[i]->edges;
        }
    }
}
void SolidEditController::CreateOrigin(){
    if(CPoint::origin != nullptr) delete CPoint::origin;
    CPoint::origin = new CPoint(nullptr);
    CPoint::origin->setName("原点");
    CPoint::origin->SetControlPoint(true);
    this->model->AddPoints(CPoint::origin);
}

