#include <QGraphicsScene>
#include "world.h"

World::World(int w, int h)
    :_clans(w*h), _w(w), _h(h)
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
                curPoint.setX(x);
                curPoint.setY(y);
                for (int i = 0; i < Clan::_size / 2; ++i)
                {
                    newPoint = curPoint;
                    newPoint += Clan::_directions[genom[i] % 8];
                    //newPoint.setX(newPoint.x() % _w);
                    //if (newPoint.x() < 0)
                    //    newPoint.setX(newPoint.x() + _w)

                    if (newPoint.x() < 0)
                        newPoint.setX(newPoint.x() % _w  + _w);
                    if (newPoint.x() >= _w)
                        newPoint.setX(newPoint.x() % _w);
                    if (newPoint.y() < 0)
                        newPoint.setY(newPoint.y() % _h  + _h);
                    if (newPoint.y() >= _h)
                        newPoint.setY(newPoint.y() % _h);
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
                clan->setRect(curPoint.x() * 5, curPoint.y() * 5, 5, 5);
            }
        }
    }
    _clans = newClans;
    qDebug() << counter;
}
