#ifndef SMARTDIMENSION_H
#define SMARTDIMENSION_H



class SmartDimension
{
public:
    enum DimensionType{
        distance1,  //距離
        distance2,  //2点距離
        angle,      //角度
        radius,     //半径
    };

private:
    DimensionType type;
    double value;

public:
    SmartDimension();
};

#endif // SMARTDIMENSION_H
