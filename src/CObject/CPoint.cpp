#include "CPoint.h"

CPoint* CPoint::origin;

CREATE_RESULT CPoint::Create(CPoint *pos){
    this->x() = pos->x();
    this->y() = pos->y();
    return CREATE_RESULT::COMPLETE;//生成終了
}

void CPoint::DrawGL(Pos camera,Pos center)const{
    if(!this->isVisible())return;
    glBegin(GL_LINE_LOOP);
    Pos cc = camera - center;
    double theta1 = std::atan2(cc.y(),std::sqrt(cc.x()*cc.x()+cc.z()*cc.z()));
    double theta2 = std::atan2(-cc.x(),cc.z());
    //円の描画
    for(double k=0;k < 2*M_PI;k += M_PI/32){
        const int length = cc.Length()*10;
        Pos p = Pos(length*std::sin(k),length*std::cos(k),0).Dot(Quat::getRotateXMatrix(theta1).Dot(Quat::getRotateYMatrix(theta2)));
        glVertex3f((p + *this).x(),(p + *this).y(),(p + *this).z());
    }
    glEnd();
}

void CPoint::MoveAbsolute(const Pos& diff){
    this->MoveRelative(diff - *this);
}
void CPoint::MoveRelative(const Pos& diff){
    if(isLock() == false && !isControlPoint() && !this->isMoving()){
        this->SetMoving(true);  //再帰呼び出し制限
        *this += diff;//移動
        emit Changed(this);
        this->SetMoving(false); //再帰呼び出し制限解除
    }
}

CObject* CPoint::GetChild(int){
    throw "CPointに子は存在しません";
}
void CPoint::SetChild(int,CObject*){
    throw "CPointに子は存在しません";
}
int CPoint::GetChildCount()const{
    return 0;
}
QVector<CPoint*> CPoint::GetAllChildren(){
    QVector<CPoint*>ans;
    ans.push_back(this);
    return ans;
}

Pos CPoint::GetNearPos(const Pos&)const{
    return *this;
}
Pos CPoint::GetNearLine(const Pos&,const Pos&)const{
    return *this;
}

CObject* CPoint::Clone()const{
    return new CPoint(static_cast<Pos>(*this));
}


CPoint::CPoint(QObject* parent):
    CObject(parent){
}
CPoint::CPoint(const Pos& origin){
    this->x() = origin.x();
    this->y() = origin.y();
    this->z() = origin.z();
}
CPoint::CPoint(const Pos &pos,QObject* parent):
    CObject(parent),
    Pos(pos){
}
CPoint::~CPoint()
{

}
CPoint& CPoint::operator=(const Pos& rhs){
    this->x() = rhs.x();
    this->y() = rhs.y();
    this->z() = rhs.z();
    return (*this);
}


