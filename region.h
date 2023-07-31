#ifndef REGION_H
#define REGION_H
#include <QColor>
class Region
{
public:
    Region();
    ~Region();
    QColor getColor();
    void setColor(QColor color);

private:
    QColor _color;
};

#endif // REGION_H
