#include <QRandomGenerator>
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

void World::getClansImage(QImage &img, DisplayMode mode)
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
            {
                if (mode == DisplayMode::Сommon)
                    img.setPixelColor(x,y,clan->getColor());
                else if (mode == DisplayMode::Food)
                {
                    float green = clan->getFood();
                    if (green < 0)
                        green = 0;
                    green /= Clan::_maxFood;
                    img.setPixelColor(x,y,QColor::fromRgbF(1 - green, green, 0, 1));
                }
                else if (mode == DisplayMode::Strength)
                {

                    img.setPixelColor(x,y,QColor(255 / static_cast<float>(Clan::_maxStrength) * clan->getStrength(),0,0,255));
                }

            }
            else
                img.setPixelColor(x,y,QColor(0,0,0,0));
        }
    }
}

void World::getRegionsImage(QImage &img, DisplayMode mode)
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

QPoint World::randomEmptySpaceNearby(QPoint pos)
{
    QVector<QPoint> seq;
    QPoint newPos;
    for (int i = 0; i < 8; ++i)
    {
        newPos = pos + Clan::_directions[i];
        returnPosToWorld(&newPos);
        if (!_clans[newPos.x() * _h + newPos.y()])
            seq.append(newPos);
    }
    if (seq.isEmpty())
        return World::ClanUndefined;
    return seq[QRandomGenerator::system()->bounded(seq.size())];
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
                    if (genom[i] == 14)
                    {
                        aimStrength(1, clan);
                    }
                    if (genom[i] == 15)
                    {
                        aimStrength(2, clan);
                    }
                    if (genom[i] == 16)
                    {
                        aimStrength(3, clan);
                    }
                    if (genom[i] == 17)
                    {
                        aimStrength(4, clan);
                    }
                }
                //приверяем выживает ли клан, если нет убираем его
                clan->survive();
                if (clan->getFood() >= 300)
                    born(pos, clan);
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
    returnPosToWorld(&newPoint);

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
        clan->increaseFood(3);
}

bool World::attack(QPoint *pos, Clan *clan)
{
    //вычисляем место атаки
    QPoint target(*pos);
    target += clan->getDirection();
    returnPosToWorld(&target);

    Clan *enemy = _clans[target.x() * _h + target.y()];
    if (enemy && enemy->getStrength() <= clan->getStrength())
    {
        _clans[target.x() * _h + target.y()] = nullptr;
        clan->increaseFood(enemy->getFood() * 2 / 3);
        enemy->kill();
        return true;
    }
    return false;
}

bool World::born(QPoint pos, Clan *clan)
{
    QPoint newPos = randomEmptySpaceNearby(pos);
    if (newPos == World::ClanUndefined)
        return false;
    Clan* son = new Clan(*clan);
    _clans[newPos.x() * _h + newPos.y()] = son;
    son->increaseFood(10);
    clan->increaseFood(-10);
    return true;
}

void World::aimStrength(int target, Clan *clan)
{
    if (clan->getStrength() != target && clan->getFood() >= 50)
    {
        if (clan->getStrength() - target > 0)
            clan->setStrength(clan->getStrength() - 1);
        else
            clan->setStrength(clan->getStrength() + 1);
        clan->increaseFood(-50);
    }
}

void World::returnPosToWorld(QPoint *pos)
{
    pos->rx() %= _w;
    if (pos->x() < 0)
        pos->rx() += _w;
    pos->ry() %= _h;
    if (pos->y() < 0)
        pos->ry() += _h;
}
