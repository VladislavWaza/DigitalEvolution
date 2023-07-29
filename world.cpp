#include "world.h"

World::World(int w, int h)
    :_clans(w*h), _w(w), _h(h)
{
}

World::~World()
{
    for (int i = 0; i < _w * _h; ++i)
        delete _clans[i];
}

int World::width()
{
    return _w;
}

int World::height()
{
    return _h;
}

bool World::addClan(int x, int y, Clan* clan)
{
    if (x < 0 || x >= _w || y < 0 || y >= _h)
        return false;
    if (_clans[x * _h + y])
        return false;
    _clans[x * _h + y] = clan;
    return true;
}

void World::getNumsOfEmptySpaces(QList<int> &list)
{
    list.clear();
    for (int i = 0; i < _w * _h; ++i)
    {
        if (!_clans[i])
            list.append(i);
    }
}

bool World::run(QImage &img)
{
    if (img.height() != _h || img.width() != _w)
        return false;
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
                if (curPoint.x() != x || curPoint.y() != y)
                {
                    img.setPixelColor(curPoint.x(),curPoint.y(),img.pixelColor(x,y));
                    img.setPixelColor(x,y,QColor(0,0,0,0));
                }
            }
        }
    }
    _clans = newClans;
    return true;
}
