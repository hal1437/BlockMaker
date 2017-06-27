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
                for(int j=0;j<edge->GetPosSequenceCount();j++){
                    right  = std::max(right ,((*edge->GetPosSequence(j)).Dot(convert)).mat[0]);
                    left   = std::min(left  ,((*edge->GetPosSequence(j)).Dot(convert)).mat[0]);
                    top    = std::max(top   ,((*edge->GetPosSequence(j)).Dot(convert)).mat[1]);
                    bottom = std::min(bottom,((*edge->GetPosSequence(j)).Dot(convert)).mat[1]);
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


CObject* SolidEditController::getHangedObject(Pos center, Pos dir)const{
    CObject* ans = nullptr;


    //点の選択
    for(CPoint* p:this->model->GetPoints()){
        if((Pos::LineNearPoint(center,center+dir, *p) - *p).Length() < CPoint::COLLISION_SIZE && hang_point != p)ans = p;
    }
    if(ans == nullptr){
        //エッジの選択
        for(CEdge* e : this->model->GetEdges()){
            if(hang_point == e->start || hang_point == e->end)continue;
            //近似点選択
            const double DIVIDE = 100;
            for(int i=0;i<=DIVIDE;i++){
                Pos p = e->GetMiddleDivide(i/DIVIDE);
                if((Pos::LineNearPoint(center,center+dir, p) - p).Length() < CPoint::COLLISION_SIZE)ans = e;
            }
        }
    }
    return ans;
}

CFace* SolidEditController::getHangedFace(Pos center,Pos camera_pos)const{
    if(this->getHangedObject(center,camera_pos-center) != nullptr) return nullptr;

    //最も近い面を選択する
    QVector<std::pair<double,CFace*>> rank;
    rank.push_back(std::make_pair(Collision::GetLengthFaceToLine(this->getFrontFace(),camera_pos,camera_pos-center),this->getFrontFace()));
    rank.push_back(std::make_pair(Collision::GetLengthFaceToLine(this->getTopFace()  ,camera_pos,camera_pos-center),this->getTopFace()));
    rank.push_back(std::make_pair(Collision::GetLengthFaceToLine(this->getSideFace() ,camera_pos,camera_pos-center),this->getSideFace()));
    for(int i=0;i<this->model->GetFaces().size();i++){
        rank.push_back(std::make_pair(Collision::GetLengthFaceToLine(this->model->GetFaces()[i],camera_pos,camera_pos-center),this->model->GetFaces()[i]));
    }
    /*
    //表示
    qDebug() << center << camera_pos;
    for(int i=0;i<rank.size();i++){
        if(Collision::CheckHitFaceToLine(rank[i].second,center,camera_pos-center)){
            Pos c = Collision::GetHitPosFaceToLine( rank[i].second->GetNorm(),
                                                   *rank[i].second->GetPoint(0),
                                                    center,camera_pos-center);
            Pos n = rank[i].second->GetNorm()*50;
            qDebug() << c;
            glBegin(GL_LINE_LOOP);
            glVertex3f((c+n).x(),(c+n).y(),(c+n).z());
            glVertex3f((c-n).x(),(c-n).y(),(c-n).z());
            glEnd();
        }
    }*/

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

void SolidEditController::Refresh3Face(){
    for(int i=0;i<3;i++){
        if(this->base[i] != nullptr) delete this->base[i];
    }
    this->base[0] = this->getFrontFace();
    this->base[1] = this->getTopFace();
    this->base[2] = this->getSideFace();
}

