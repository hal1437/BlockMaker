
#include "TimeDivider.h"

void TimeDivider::step(){
    this->now += TIME_DIVIDE;//時間遷移
    emit PerTime();
    if(this->now > this->goal){
        obj = end;
        this->stop();//終了
    }else{
        obj = (end - start) * (now/goal) + start;//遷移
    }
}

void TimeDivider::run(){
    this->timer = new QTimer(this);
    connect(this->timer,SIGNAL(timeout()),this,SLOT(step()));
    this->timer->start(TIME_DIVIDE);
    obj = start;
}
void TimeDivider::stop(){
    this->timer->stop();
    disconnect(this->timer,SIGNAL(timeout()),this,SLOT(step()));
    this->timer = nullptr;
    emit EndTime();
}

void TimeDivider::setStart(double start){
    this->start = start;
}
void TimeDivider::setEnd  (double end){
    this->end   = end;
}

TimeDivider::TimeDivider(double& obj):
    obj(obj)
{
    timer = nullptr;
}
TimeDivider::TimeDivider(double& obj,double start,double end,double goal):
    obj(obj),start(start),end(end),goal(goal)
{
    timer = nullptr;
}

TimeDivider::~TimeDivider(){
    this->stop();
}

