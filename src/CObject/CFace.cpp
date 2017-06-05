#include "CFace.h"


bool CFace::isParadox()const{
    if(this->corner.size() < 2)return false;
    else {
        //矛盾がないか確認する。
        for(int i=2;i<this->corner.size();i++){
            if(this->isComprehension(*this->corner[i]) == false)return false;
        }
    }
    return true;
}

bool CFace::isComprehension(Pos pos)const{
    if(this->corner.size()<2)return true;
    else {
        //法線ベクトルとの内積が0であれば平面上に存在する。
        return this->GetNorm().DotPos(pos) == 0;
    }
}

Pos CFace::GetNorm()const{
    if(this->corner.size()<2)return Pos();
    else                     return Pos(this->corner[0]->Cross(*this->corner[1])).GetNormalize();
}


bool CFace::Draw(QPainter& painter)const{
    return true;
}
bool CFace::DrawGL(Pos,Pos)const{
    glBegin(GL_LINE_LOOP);
    for(CPoint* c : this->corner){
        glVertex3f(c->x(),c->y(), c->z());
    }
    glEnd();

    return true;
}

bool CFace::Move(const Pos& diff){
    return true;
}


//近接点
Pos CFace::GetNearPos (const Pos& hand)const{
    return Pos();
}
Pos CFace::GetNearLine(const Pos& pos1,const Pos& pos2)const{
    return Pos();
}



CFace::CFace(QObject* parent):
    CObject(parent)
{
}

CFace::~CFace()
{
}

