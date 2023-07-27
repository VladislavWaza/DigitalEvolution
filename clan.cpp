#include <QRandomGenerator>
#include <QGraphicsScene>
#include "clan.h"

Clan::Clan(GenomeInitType genomeInitType, QGraphicsItem *parent)
    :QGraphicsRectItem(parent)
{
    _genom = new uint8_t[_size];
    if (genomeInitType == GenomeInitType::Random)
    {
        for (int i = 0; i < _size/4; ++i)
            *reinterpret_cast<uint32_t*>(&_genom[4*i]) = QRandomGenerator::system()->generate();
    }
}

Clan::~Clan()
{
}

void Clan::run()
{
    for (int i = 0; i < _size; ++i)
    {
        QPointF curPos = rect().topLeft();
        QPointF target;
        if (_genom[i] % 9 == 0)
        {
           target = curPos + QPointF(-5, 0);
        }
        if (_genom[i] % 9 == 1)
        {
           target = curPos + QPointF(-5, -5);
        }
        if (_genom[i] % 9 == 2)
        {
           target = curPos + QPointF(0, -5);
        }
        if (_genom[i] % 9 == 3)
        {
           target = curPos + QPointF(5, -5);
        }
        if (_genom[i] % 9 == 4)
        {
           target = curPos + QPointF(5, 0);
        }
        if (_genom[i] % 9 == 5)
        {
           target = curPos + QPointF(5, 5);
        }
        if (_genom[i] % 9 == 6)
        {
           target = curPos + QPointF(0, 5);
        }
        if (_genom[i] % 9 == 7)
        {
           target = curPos + QPointF(-5, 5);
        }
        if (_genom[i] % 9 != 8)
        {
           int s = scene()->items(target + QPointF(1, 1)).size();
           if (s == 1)
           {
               setRect(target.x(), target.y(), 5, 5);
           }
        }
    }
}
