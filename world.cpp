#include "world.h"

QPoint const World::ClanUndefined = QPoint(-1,-1);


World::World(int w, int h)
    :_clans(w*h), _regions(w*h), _w(w), _h(h)
{
}

World::~World()
{
    for (int i = 0; i < _w * _h; ++i)
    {
        delete _clans[i];
        delete _regions[i];
    }

}

int World::width()
{
    return _w;
}

int World::height()
{
    return _h;
}

Clan *World::getClan(int x, int y)
{
    if (x < 0 || x >= _w || y < 0 || y >= _h)
        return nullptr;
    return _clans[x * _h + y];
}

QPoint World::getClanPos(Clan *clan)
{
    if (!clan)
        return QPoint(-1,-1);
    for (int x = 0; x < _w; ++x)
    {
        for (int y = 0; y < _h; ++y)
        {
            if (_clans[x * _h + y] == clan)
                return QPoint(x,y);
        }
    }
    return QPoint(-1,-1);
}

Region *World::getRegion(int x, int y)
{
    if (x < 0 || x >= _w || y < 0 || y >= _h)
        return nullptr;
    return _regions[x * _h + y];
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

bool World::setRegion(int x, int y, Region *region)
{
    if (x < 0 || x >= _w || y < 0 || y >= _h)
        return false;
    if (_regions[x * _h + y])
        delete _regions[x * _h + y];
    _regions[x * _h + y] = region;
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

int World::clansNumber()
{
    int n = 0;
    for (int i = 0; i < _w * _h; ++i)
    {
        if (_clans[i])
            ++n;
    }
    return n;
}

bool World::run(QImage &img)
{
    if (img.height() != _h || img.width() != _w)
        return false;
    QList<Clan*> newClans(_clans);
    Clan *clan;
    uint8_t genom[Clan::_size];
    QPoint pos;

    for (int x = 0; x < _w; ++x)
    {
        for (int y = 0; y < _h; ++y)
        {
            clan = _clans[x * _h + y];
            if (clan && clan->isAlive())
            {
                pos.rx() = x;
                pos.ry() = y;
                clan->getGenom(genom);
                for (int i = 0; i < Clan::_size; ++i)
                {
                    if (genom[i] >= 0 && genom[i] <= 7)
                        clan->setDirection(Clan::_directions[genom[i]]);
                    if (genom[i] == 8)
                    {
                        if (move(&pos, clan, img, newClans))
                            break;
                    }
                    if (genom[i] == 9)
                    {
                        collectFood(pos, clan);
                        break;
                    }
                    if (genom[i] == 10)
                    {
                        if (attack(&pos, clan, img, newClans))
                            break;
                    }
                }
                clan->survive();
                if (!clan->isAlive())
                {
                    img.setPixelColor(pos.x(),pos.y(),QColor(0,0,0,0));
                    newClans[pos.x() * _h + pos.y()] = nullptr;
                }
            }
        }
    }
    for (int x = 0; x < _w; ++x)
    {
        for (int y = 0; y < _h; ++y)
        {
            clan = _clans[x * _h + y];
            if (clan && !clan->isAlive())
            {
                delete clan;
            }
        }
    }
    _clans = newClans;
    return true;
}

bool World::move(QPoint *pos, Clan *clan, QImage &img, QList<Clan *> &newClans)
{
    QPoint newPoint(*pos);
    newPoint += clan->getDirection();

    newPoint.rx() %= _w;
    if (newPoint.x() < 0)
        newPoint.rx() += _w;
    newPoint.ry() %= _h;
    if (newPoint.y() < 0)
        newPoint.ry() += _h;

    if (!newClans[newPoint.x() * _h + newPoint.y()])
    {
        newClans[newPoint.x() * _h + newPoint.y()] = clan;
        newClans[pos->x() * _h + pos->y()] = nullptr;
        img.setPixelColor(newPoint.x(),newPoint.y(),img.pixelColor(pos->x(),pos->y()));
        img.setPixelColor(pos->x(),pos->y(),QColor(0,0,0,0));
        *pos = newPoint;
        return true;
    }
    return false;
}

void World::collectFood(QPoint pos, Clan *clan)
{
    clan->increaseFood(200);
}

bool World::attack(QPoint *pos, Clan *clan, QImage &img, QList<Clan*> &newClans)
{
    QPoint target(*pos);
    target += clan->getDirection();
    target.rx() %= _w;
    if (target.x() < 0)
        target.rx() += _w;
    target.ry() %= _h;
    if (target.y() < 0)
        target.ry() += _h;

    Clan *enemy = newClans[target.x() * _h + target.y()];

    if (enemy)
    {
        if (clan->getPopulation() >= enemy->getPopulation())
        {
            newClans[target.x() * _h + target.y()] = nullptr;
            img.setPixelColor(target.x(),target.y(),QColor(0,0,0,0));
            clan->increaseFood(enemy->getFood());
            enemy->kill();
        }
        return true;
    }
    return false;
}
