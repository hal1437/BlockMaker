#ifndef TIMEDIVIDER_H
#define TIMEDIVIDER_H

#include <QObject>
#include <QTimer>
#include <QDebug>

//条件
//以下の演算子を定義
// +double /double *double

//時間遷移
class TimeDivider :public QObject
{
    Q_OBJECT

public:
    constexpr static double TIME_DIVIDE = 30;//デフォルト分割時間
private:
    double& obj; //対象
    double  start;//初期状態
    double  end;  //目標
    double  now;//現在時刻
    double  goal;//目標
    QTimer* timer;//タイマー

public:
    //時間遷移開始
    template<class T>
    static TimeDivider* TimeDivide(T& obj,const double to,double time){
        TimeDivider* ans = new TimeDivider(obj,obj,to,time);
        ans->run();
        return ans;
    }

private slots:
    void step();

public slots:
    //実行中判定
    bool isRunning()const{return this->timer!=nullptr;}

    //実行
    void run();
    void stop();

    //取得
    void setStart(double start);
    void setEnd  (double end  );
public:
    TimeDivider(double& obj);
    TimeDivider(double& obj,double start,double end,double goal);
    ~TimeDivider();

signals:
    void EndTime();
    void PerTime();
};

#endif // TIMEDIVIDER_H
