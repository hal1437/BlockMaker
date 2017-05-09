#include "GridFilter.h"

//フィルタリング
Pos GridFilter::Filtering(Pos p)const{

    Pos answer;
    if(this->x_grid == 0)answer.x = p.x;
    else                 answer.x = this->x_grid * std::round(p.x/this->x_grid);
    if(this->y_grid == 0)answer.y = p.y;
    else                 answer.y = this->y_grid * std::round(p.y/this->y_grid);
    return answer;
}

void GridFilter::DrawGrid(QPainter &p, int x, int y, int w, int h)const{

    x = this->Filtering(Pos(x,y)).x;
    y = this->Filtering(Pos(x,y)).y;

    //点線に変更
    QPen pen = p.pen();
    pen.setStyle(Qt::DashLine);
    p.setPen(pen);

    const double over = 100;

    //過剰すぎなければ描画
    if((h / this->y_grid) < 200){
        //横線
        for(int i=0;this->y_grid*i<=h;i++){
            p.drawLine(QPointF(x   - over,this->y_grid*i+y),
                       QPointF(w+x + over,this->y_grid*i+y));
        }
    }
    if((w / this->x_grid) < 200){
        //縦線
        for(int i=0;this->x_grid*i<=w;i++){
            p.drawLine(QPointF(this->x_grid*i+x,y   - over),
                       QPointF(this->x_grid*i+x,h+y + over));
        }
    }
}


void GridFilter::setXGrid(double value){
    this->x_grid = value;
}

void GridFilter::setYGrid(double value){
    this->y_grid = value;
}

GridFilter::GridFilter():
    x_grid(0),y_grid(0){
}

GridFilter::GridFilter(double x,double y):
    x_grid(x),y_grid(y){

}

GridFilter::~GridFilter()
{

}

