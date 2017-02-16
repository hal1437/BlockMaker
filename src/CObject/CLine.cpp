#include "CLine.h"

bool CLine::Create(CPoint *pos, int index){
    if(index == 0 || index == 1){
        this->pos[index] = pos;
        if(index==0){
            return false;//継続
        }
        if(index==1){
            return true; //終了
        }
    }
    return false;
}

Pos CLine::GetNear(const Pos& hand)const{
    return Pos::LineNearPoint(GetJointPos(0),GetJointPos(1),hand);
}

bool CLine::Draw(QPainter& painter,QTransform trans)const{
    Pos pos1,pos2;
    pos1 = GetJointPos(0).Transform(trans);

    if(this->isCreating()){
        //製作ならばマウスの位置を1番目にする
        pos2 = this->mouse_over.Transform(trans);
    }else{
        //二点間の直線
        pos2 = GetJointPos(1).Transform(trans);
    }
    //描画
    painter.drawLine(pos1.x,pos1.y,pos2.x,pos2.y);

    //ロックマーク
    /*
    if(this->isLock()){
        Pos p = (GetJointPos(0) - GetJointPos(1))/2 + GetJointPos(1);
        p.Transform(trans);
        painter.drawImage(p.x+10,p.y-10,QImage(":/Restraint/LockRestraint.png"));
    }*/

    return true;
}
bool CLine::isSelectable()const{
    //直線と点の距離のアルゴリズムによって選択を決定
    float d;
    Pos v1,v2,near;
    v1   = GetJointPos(1) - GetJointPos(0);
    v2   = mouse_over  - GetJointPos(0);
    near = Pos::LineNearPoint(Pos(),v1,v2);
    d    = (near - v2).Length();

    //あたり判定処理
    if(d < COLLISION_SIZE / CObject::Drawing_scale &&
       ((GetJointPos(0).x < mouse_over.x && mouse_over.x < GetJointPos(1).x) ||
        (GetJointPos(1).x < mouse_over.x && mouse_over.x < GetJointPos(0).x) ||
        (GetJointPos(0).y < mouse_over.y && mouse_over.y < GetJointPos(1).y) ||
        (GetJointPos(1).y < mouse_over.y && mouse_over.y < GetJointPos(0).y))){

        return true;
    }else{
        return false;
    }
}

void CLine::Lock(bool lock){
    //それぞれロック
    pos[0]->Lock(lock);
    pos[1]->Lock(lock);
    CObject::Lock(pos[0]->isLock() & pos[1]->isLock());
}

bool CLine::Move(const Pos& diff){
    //子に伝達
    for(int i = 0;i<2;i++){
        //ロックしていない
        if(!pos[i]->isLock()){
            *pos[i] += diff;
        }
    }
    return true;
}


int CLine::GetJointNum()const{
    return 2;
}
Pos CLine::GetJointPos(int index)const{
    if(index == 1 && this->isCreating())return CObject::mouse_over;
    return *pos[index];
}
CPoint* CLine::GetJoint(int index){
    return pos[index];
}
std::vector<CObject*> CLine::GetChild(){
    std::vector<CObject*> ans;
    if(this->isCreating())return ans;
    ans.push_back(pos[0]);
    ans.push_back(pos[1]);
    return ans;
}

double CLine::DistanceToPoint(const Pos& pos)const{
    Pos AP = pos - this ->GetJointPos(0);
    Pos AB = pos - this ->GetJointPos(1);
    double AB_l = AB.Length() * AB.Length();
    Pos I = this->GetJointPos(0) + AB * (AP.Dot(AB) / AB_l);
    return (I - pos).Length();
}


CLine::CLine()
{
    pos[0] = pos[1] = nullptr;
}

CLine::~CLine()
{

}

