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

void World::getClansImage(QImage &img)
{
    img = img.scaled(_w, _h);
    img = img.convertedTo(QImage::Format_ARGB32);
    Clan *clan;
    for (int x = 0; x < _w; ++x)
    {
        for (int y = 0; y < _h; ++y)
        {
            clan = _clans[x * _h + y];
            if (clan)
                img.setPixelColor(x,y,clan->getColor());
            else
                img.setPixelColor(x,y,QColor(0,0,0,0));
        }
    }
}

void World::getRegionsImage(QImage &img)
{
    img = img.scaled(_w, _h);
    img = img.convertedTo(QImage::Format_ARGB32);
    Region *region;
    for (int x = 0; x < _w; ++x)
    {
        for (int y = 0; y < _h; ++y)
        {
            region = _regions[x * _h + y];
            if (region)
                img.setPixelColor(x,y,region->getColor());
            else
                img.setPixelColor(x,y,QColor(0,0,0,0));
        }
    }
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

void World::run()
{
    QList<Clan*> oldClans(_clans);
    Clan *clan;
    uint8_t genom[Clan::_size];
    QPoint pos;

    for (int x = 0; x < _w; ++x)
    {
        for (int y = 0; y < _h; ++y)
        {
            clan = oldClans[x * _h + y];
            if (clan && clan->isAlive())
            {
                pos.rx() = x;
                pos.ry() = y;
                clan->getGenom(genom);

                //выполняем команды
                for (int i = 0; i < Clan::_size; ++i)
                {
                    if (genom[i] >= 0 && genom[i] <= 7)
                        clan->setDirection(Clan::_directions[genom[i]]);
                    if (genom[i] == 8 || genom[i] == 9)
                    {
                        if (move(&pos, clan))
                            break;
                    }
                    if (genom[i] == 10 || genom[i] == 11)
                    {
                        collectFood(pos, clan);
                        break;
                    }
                    if (genom[i] == 12 || genom[i] == 13)
                    {
                        if (attack(&pos, clan))
                            break;
                    }
                }

                //приверяем выживает ли клан, если нет убираем его
                clan->survive();
                if (!clan->isAlive())
                {
                    _clans[pos.x() * _h + pos.y()] = nullptr;
                }
            }
        }
    }

    //удаляем мертвые кланы
    for (int x = 0; x < _w; ++x)
    {
        for (int y = 0; y < _h; ++y)
        {
            clan = oldClans[x * _h + y];
            if (clan && !clan->isAlive())
            {
                delete clan;
            }
        }
    }
}

bool World::move(QPoint *pos, Clan *clan)
{
    //вычисляем место перемещения
    QPoint newPoint(*pos);
    newPoint += clan->getDirection();
    newPoint.rx() %= _w;
    if (newPoint.x() < 0)
        newPoint.rx() += _w;
    newPoint.ry() %= _h;
    if (newPoint.y() < 0)
        newPoint.ry() += _h;

    if (!_clans[newPoint.x() * _h + newPoint.y()]) //если не занято
    {
        _clans[newPoint.x() * _h + newPoint.y()] = clan; //перемещаемся
        _clans[pos->x() * _h + pos->y()] = nullptr; //прошлое место опустошаем
        *pos = newPoint;
        return true;
    }
    return false;
}

void World::collectFood(QPoint pos, Clan *clan)
{
    clan->increaseFood(100);
}

bool World::attack(QPoint *pos, Clan *clan)
{
    //вычисляем место атаки
    QPoint target(*pos);
    target += clan->getDirection();
    target.rx() %= _w;
    if (target.x() < 0)
        target.rx() += _w;
    target.ry() %= _h;
    if (target.y() < 0)
        target.ry() += _h;

    Clan *enemy = _clans[target.x() * _h + target.y()];
    if (enemy)
    {
        _clans[target.x() * _h + target.y()] = nullptr;
        clan->increaseFood(enemy->getFood());
        enemy->kill();
        return true;
    }
    return false;
}
