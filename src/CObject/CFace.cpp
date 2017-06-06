#include "CFace.h"


bool CFace::Creatable(QVector<CObject*> pos){
    if(std::any_of(pos.begin(),pos.end(),[](CObject* p){return !p->is<CPoint>();}))return false;
    if(pos.size()<=2)return false;
    if(pos.size()==3)return true;

    Pos cross = (*dynamic_cast<CPoint*>(pos[1]) - *dynamic_cast<CPoint*>(pos[0])).Cross
                (*dynamic_cast<CPoint*>(pos[2]) - *dynamic_cast<CPoint*>(pos[0]));
    for(int i=3;i<pos.size();i++){
        double d = dynamic_cast<CPoint*>(pos[i])->DotPos(cross);
        if(!NearlyEqual(d,0))return false;
    }
    return true;
}

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
    else{
        return Pos((*this->corner[                    1] - *this->corner[0]).Cross
                   (*this->corner[this->corner.size()-1] - *this->corner[0])).GetNormalize();
    }
}


bool CFace::Draw(QPainter& painter)const{
    return true;
}
bool CFace::DrawGL(Pos,Pos)const{
    if(this->isPolygon()){
        //薄い色に変更
        float currentColor[4];
        glGetFloatv(GL_CURRENT_COLOR,currentColor);
        glColor4f(currentColor[0],currentColor[1],currentColor[2], 0.1);

        //中を塗る
        glBegin(GL_TRIANGLE_FAN);

        for(CPoint* c : this->corner){
                glVertex3f(c->x(),c->y(), c->z());
        }
        glEnd();

        //色を復元
        glColor4f(currentColor[0],currentColor[1],currentColor[2], currentColor[3]);
    }

    //外側
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

