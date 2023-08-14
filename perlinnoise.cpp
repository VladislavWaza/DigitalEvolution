#include <QRandomGenerator>
#include "perlinnoise.h"

PerlinNoise::PerlinNoise(int height, int width, int ratio)
{
    _h = height;
    _w = width;
    _matrixSide = ratio + 1;
    _stepHeight = static_cast<float>(_h) / ratio;
    _stepWidth = static_cast<float>(_w) / ratio;

    QRandomGenerator rand(QRandomGenerator::system()->generate());
    for (int i = 0; i < _matrixSide * _matrixSide; ++i)
    {
        double angle = rand.generateDouble() * 360;
        _gradients.append(QPointF(cos(angle), sin(angle)));
    }
}

float PerlinNoise::noise(float x, float y)
{
    //выпишем градиенты на углах клетки
    QPointF gradientLU, gradientRU, gradientRD, gradientLD;
    int XinMatrix = static_cast<int>(x / _stepWidth);
    int YinMatrix = static_cast<int>(y / _stepHeight);
    gradientLU = _gradients[XinMatrix * _matrixSide + YinMatrix];
    gradientRU = _gradients[(XinMatrix + 1) * _matrixSide + YinMatrix];
    gradientRD = _gradients[(XinMatrix + 1) * _matrixSide + YinMatrix + 1];
    gradientLD = _gradients[XinMatrix * _matrixSide + YinMatrix + 1];

    //перейдем к координатам отностельно верхнего левого угла клетки
    x = fmodf(x, _stepWidth);
    y = fmodf(y, _stepHeight);

    //пронормируем локальные координаты
    float normX = x / _stepWidth;
    float normY = y / _stepHeight;

    //вектора указывающие на нашу точку от углов клетки
    QPointF vecLU, vecRU, vecRD, vecLD;
    vecLU = QPointF(normX,normY);
    vecRU = QPointF(normX - 1,normY);
    vecRD = QPointF(normX - 1,normY - 1);
    vecLD = QPointF(normX,normY - 1);

    //уровни шума в углах клетки
    float scalarLU, scalarRU, scalarRD, scalarLD;
    scalarLU = QPointF::dotProduct(gradientLU, vecLU);
    scalarRU = QPointF::dotProduct(gradientRU, vecRU);
    scalarRD = QPointF::dotProduct(gradientRD, vecRD);
    scalarLD = QPointF::dotProduct(gradientLD, vecLD);

    //результат линейной интерполяции верхней и нижней грани
    float up = lerp(scalarLU, scalarRU, quintic(normX));
    float down = lerp(scalarLD, scalarRD, quintic(normX));

    return lerp(up, down, quintic(normY));
}

float PerlinNoise::quintic(float t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoise::lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}
