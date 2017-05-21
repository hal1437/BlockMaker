#include "CPoint.h"

CREATE_RESULT CPoint::Create(CPoint *pos){
    this->x() = pos->x();
    this->y() = pos->y();
    return CREATE_RESULT::COMPLETE;//生成終了
}

bool CPoint::Draw(QPainter& painter)const{
    Pos center;
    QTransform conv;

    //CPointの円の大きさ・太さは、拡大によらず常に一定である。
    painter.save();
    conv = painter.transform();
    center = Pos(*this).Transform(conv);

    //描画設定
    painter.setTransform(QTransform());//変換行列解除
    painter.setPen(QPen(painter.pen().color(), CObject::DRAWING_LINE_SIZE));

    if(control_point){//制御点ならば
        //四角
        QLineF ps[4] = {QLineF(center.x()-DRAWING_CIRCLE_SIZE,center.y()-DRAWING_CIRCLE_SIZE,center.x()+DRAWING_CIRCLE_SIZE,center.y()-DRAWING_CIRCLE_SIZE),
                        QLineF(center.x()+DRAWING_CIRCLE_SIZE,center.y()-DRAWING_CIRCLE_SIZE,center.x()+DRAWING_CIRCLE_SIZE,center.y()+DRAWING_CIRCLE_SIZE),
                        QLineF(center.x()+DRAWING_CIRCLE_SIZE,center.y()+DRAWING_CIRCLE_SIZE,center.x()-DRAWING_CIRCLE_SIZE,center.y()+DRAWING_CIRCLE_SIZE),
                        QLineF(center.x()-DRAWING_CIRCLE_SIZE,center.y()+DRAWING_CIRCLE_SIZE,center.x()-DRAWING_CIRCLE_SIZE,center.y()-DRAWING_CIRCLE_SIZE)};
        painter.drawLines(ps,4);
    }else{
        QRectF rect(center.x()-DRAWING_CIRCLE_SIZE,
                    center.y()-DRAWING_CIRCLE_SIZE,
                    DRAWING_CIRCLE_SIZE*2,
                    DRAWING_CIRCLE_SIZE*2);
        //丸
        painter.drawArc(rect,0,360*16);
    }
    //ロック時
    if(this->isLock() && !this->isControlPoint()){
        painter.drawImage(center.x()+10,center.y()-10,QImage(":/Restraint/LockRestraint.png"));
    }

    //復元
    painter.restore();

    return true;
}


bool CPoint::Move(const Pos& diff){
    //ロックされていなければ
    if(isLock() == false && !isControlPoint()){
        if(diff != Pos(0,0) && moving == false){
            //シグナル
            moving = true;  //再帰呼び出し制限
            *this += diff;//単純な平行移動
            emit PosChanged(this,*this-diff);
            moving = false; //再帰呼び出し制限
        }
    }
    return true;
}

bool CPoint::isLock()const{
    return (this->lock || this->isControlPoint());
}

bool CPoint::isControlPoint()const{
    return this->control_point;
}

bool CPoint::ControlPoint(bool f){
    control_point = f;
    return true;
}

Pos CPoint::GetNear(const Pos&)const{
    return *this;
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

CPoint::CPoint(double x, double y, double z, QObject *parent):
    CObject(parent),
    Pos(x,y,z){

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
