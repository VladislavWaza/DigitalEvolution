#include <QGraphicsScene>
#include "world.h"

World::World(int squareSide, int w, int h)
    :_clans(w*h), _w(w), _h(h), _squareSide(squareSide)
{
}

void World::addItem(int x, int y, Clan* clan)
{
    _clans[x * _h + y] = clan;
}

void World::run()
{
    int counter = 0;
    QList<Clan*> newClans(_clans);
    QPoint curPoint, newPoint;
    Clan *clan;
    uint8_t genom[Clan::_size];
    for (int x = 0; x < _w; ++x)
    {
        for (int y = 0; y < _h; ++y)
        {
            clan = _clans[x * _h + y];
            if (clan)
            {
                ++counter;
                clan->getGenom(genom);
                curPoint.rx() = x;
                curPoint.ry() = y;
                for (int i = 0; i < Clan::_size; ++i)
                {
                    newPoint = curPoint;
                    newPoint += Clan::_directions[genom[i] % 8];

                    newPoint.rx() %= _w;
                    if (newPoint.x() < 0)
                        newPoint.rx() += _w;
                    newPoint.ry() %= _h;
                    if (newPoint.y() < 0)
                        newPoint.ry() += _h;

                    if (!newClans[newPoint.x() * _h + newPoint.y()])
                    {
                        newClans[newPoint.x() * _h + newPoint.y()] = clan;
                        newClans[curPoint.x() * _h + curPoint.y()] = nullptr;
                        curPoint = newPoint;
                    }
                    else
                    {
                        newClans[curPoint.x() * _h + curPoint.y()] = clan;
                    }
                }
                clan->setRect(curPoint.x() * _squareSide, curPoint.y() * _squareSide, _squareSide, _squareSide);
            }
        }
    }
    _clans = newClans;
    qDebug() << counter;
}
