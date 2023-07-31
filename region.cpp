#include "region.h"

Region::Region()
{
    _color = Qt::gray;
}

Region::~Region()
{
}

QColor Region::getColor()
{
    return _color;
}

void Region::setColor(QColor color)
{
    _color = color;
}
