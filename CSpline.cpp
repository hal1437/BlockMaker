#include "CSpline.h"

CSpline::CSpline()
{

}

CSpline::~CSpline()
{

}

bool CSpline::Create(Relative<Pos> pos, int index){
    if(0 <= index){
        this->pos.push_back(pos);
        return false;
        /*
        if(index==1){
            this->is_Creating = false;
            return true;
        }else{
            this->is_Creating = true;
            return false;
        }*/
    }
    this->is_Creating = false;
    return false;
}

Pos CSpline::GetNear(const Pos& hand)const{
    return Pos();
}

bool CSpline::Draw(QPainter& painter)const{
    if(pos.size() >= 2){
        //簡易曲線のため完全なスプライン曲線の実装をすること
        QPainterPath path;
        path.moveTo(pos[0]().x,pos[0]().y);
        Pos c_now ;
        Pos c_next = (pos[1]() - pos[0]())/5.0;
        for(int i=0;i<pos.size()-1;i++){
            c_now  = -c_next;
            //if(i != pos.size()-1)c_next = (pos[i]()-pos[i+1]())/3.0;
            //else c_next = Pos();
            path.cubicTo((c_now +pos[i]()).x,(c_now +pos[i]()).y,
                         pos[i+1]().x,pos[i+1]().y,
                         pos[i+1]().x,pos[i+1]().y);
        }
        painter.drawPath(path);
    }
    return true;
}
bool CSpline::Selecting(){
    //スプラインと点の距離のアルゴリズム
/*
    Pos dir1 = pos[0]() - center();
    Pos dir2 = pos[1]() - center();
    Pos dir3 = CObject::mouse_over - center();

    float d = dir3.Length();
    if((d < COLLISION_SIZE || (std::abs(d-round) < COLLISION_SIZE)) &&
       (Pos::Angle(dir2,dir3) < Pos::Angle(dir2,dir1))){
        return true;
    }else{
        return false;
    }
    */
    return false;
}

bool CSpline::isLocked(){
    return false;
}
bool CSpline::Move(const Pos& diff){
    for(int i=0;i<pos.size();i++){
        pos[i].getReference()->diff += diff;
    }
    return true;
}

int CSpline::GetJointNum()const{
    return this->pos.size();
}
Pos CSpline::GetJointPos(int index)const{
    return pos[index]();
}


