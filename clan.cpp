#include <QRandomGenerator>
#include "clan.h"

QPoint const Clan::_directions[] = {QPoint(-1, 0), QPoint(-1, -1), QPoint(0, -1),
                                QPoint(1, -1), QPoint(1, 0), QPoint(1, 1), QPoint(0, 1), QPoint(-1, 1)};

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

Clan::getGenom(uint8_t *ptr) const
{
    for (int i = 0; i < _size; ++i)
        ptr[i] = _genom[i];
}
