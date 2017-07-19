#ifndef GRIDFILTER_H
#define GRIDFILTER_H

#include <QPainter>
#include <Point.h>
#include <Utils.h>

class GridFilter : public QObject
{
private:
    double x_grid;
    double y_grid;
public:

    //フィルタリング
    Pos Filtering(Pos p)const;
    //グリッド描画
    void DrawGrid(QPainter& p,int x,int y,int w,int h)const;

    void setXGrid(double value);
    void setYGrid(double value);

    GridFilter();
    GridFilter(double x,double y);
    ~GridFilter();


};

#endif // GRIDFILTER_H
