#include "region.h"

Region::Region(float light)
{
    _light = light * _maxLight;
}

Region::~Region()
{
}

float Region::getLight()
{
    return _light;
}
