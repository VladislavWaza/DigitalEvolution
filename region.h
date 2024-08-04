#ifndef REGION_H
#define REGION_H
#include <QColor>
class Region
{
public:
    /*принимет light от 0 до 1 и сам переносит на 0 - _maxLight*/
    Region(float light) : _light(light * _maxLight) {}
    ~Region() = default;
    float getLight() const {return _light;}

    static const int _maxLight = 5;
private:
    float _light;
};

#endif // REGION_H
