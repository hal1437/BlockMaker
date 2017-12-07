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
    face->SetVisibleDetail(false);

    double top=0,bottom=0,right=0,left=0;
    for(CPoint* pos:this->model->GetPoints()){
        right  = std::max(right ,(pos->Dot(convert)).mat[0]);
        left   = std::min(left  ,(pos->Dot(convert)).mat[0]);
        top    = std::max(top   ,(pos->Dot(convert)).mat[1]);
        bottom = std::min(bottom,(pos->Dot(convert)).mat[1]);
    }
    //何もない時
    if(this->model->GetPoints().empty() ||
       (top==0 && bottom==0 && right==0 && left==0)){
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
    return (this->projection_face != nullptr);
}

CObject* SolidEditController::getHangedObject(Pos center, Pos dir,double zoom_rate)const{
    QVector<QPair<CObject*,double>>ans;

    //カメラ座標と方向を2点に変換
    Pos pos1 = center;
    Pos pos2 = center+dir;
    //点の選択
    for(CPoint* p:this->model->GetPoints()){
        //不可視ならパス
        if(!p->isVisible())continue;
       //最短距離
        double length = (Pos::LineNearPoint(pos1,pos2, *p) - *p).Length() / zoom_rate;

        //範囲内かつ、つかみ点でない
        if(length < CPoint::COLLISION_SIZE && CPoint::hanged != p){
            //スケッチ中なら、平面上に存在する条件を追加
            if(this->isSketcheing()){
                if(this->projection_face->isComprehension(*p)){
                    ans.push_back(qMakePair(p,length));
                }
            }else{
                ans.push_back(qMakePair(p,length));
            }
        }
    }

    //点が一つも選択されていなければ
    if(ans.size() == 0){
        //エッジの選択
        for(CEdge* e : this->model->GetEdges()){
            //例外として選択不可
            if(CPoint::hanged == e->start || CPoint::hanged == e->end || !e->isVisible())continue;

            //近似点選択
            Pos near = e->GetNearLine(center,center+dir);
            //CPoint* pp = new CPoint(near);
            //pp->DrawGL(center+dir,center);

            //最短距離
            double length = (Pos::LineNearPoint(pos1,pos2, near) - near).Length() / zoom_rate;

            if(length  < CLine::COLLISION_SIZE){
                //スケッチ中なら、平面上に存在する条件を追加
                if(this->isSketcheing()){
                    if(this->projection_face->isComprehension(*e->start) &&
                       this->projection_face->isComprehension(*e->end)){
                        ans.push_back(qMakePair(e,length));
                    }
                }else{
                    ans.push_back(qMakePair(e,length));
                }
            }
        }
    }
    if(ans.size() == 0)return nullptr;
    else{
        //複数の候補が存在する場合は、もっとも近い点のものを選択する。
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

    //三平面以外が含まれていれば
    if(std::any_of(rank.begin(),rank.end(),[](std::pair<double,CFace*> rank_v){
        return !exist(CFace::base,rank_v.second);
    })){
        //三平面を削除
        for(int i=0;i<rank.size();i++){
            if(exist(CFace::base,rank[i].second)){
                rank.removeAll(rank[i]);
                i--;
            }
        }
    }
    //存在しなければぬるぽ
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
            CFace::base[i]->SetContours(true);
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

