#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include <QList>
#include <QPointF>

class PerlinNoise
{
public:
    PerlinNoise(int height, int width, int ratio);

    float noise(float x, float y);
private:
    float quintic(float t);
    float lerp(float a, float b, float t);

    int _h, _w, _matrixSide;
    float _stepHeight, _stepWidth;
    QList<QPointF> _gradients;
};

#endif // PERLINNOISE_H
