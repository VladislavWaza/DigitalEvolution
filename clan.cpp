#include <QRandomGenerator>
#include "clan.h"

QPoint const Clan::_directions[] = {QPoint(-1, 0), QPoint(-1, -1), QPoint(0, -1),
                                QPoint(1, -1), QPoint(1, 0), QPoint(1, 1), QPoint(0, 1), QPoint(-1, 1)};

Clan::Clan(GenomeInitType genomeInitType)
{
    _genom = new uint8_t[_size];
    if (genomeInitType == GenomeInitType::Random)
    {
        for (int i = 0; i < _size/4; ++i)
            *reinterpret_cast<uint32_t*>(&_genom[4*i]) = QRandomGenerator::system()->generate();
    }
    for (int i = 0; i < _size; ++i)
        _genom[i] %= 11;
    _food = 0;
    _population = 100;
    _direction = Clan::_directions[0];
    _isAlive = true;
}

Clan::~Clan()
{
}

void Clan::getGenom(uint8_t *ptr) const
{
    for (int i = 0; i < _size; ++i)
        ptr[i] = _genom[i];
}

void Clan::setDirection(QPoint direction)
{
    _direction = direction;
}

QPoint Clan::getDirection()
{
    return _direction;
}

int Clan::getPopulation()
{
    return _population;
}

int Clan::getFood()
{
    return _food;
}

void Clan::increaseFood(int food)
{
    _food += food;
}

bool Clan::isAlive()
{
    return _isAlive;
}

void Clan::kill()
{
    _isAlive = false;
    emit signalKilled();
}

void Clan::survive()
{
    if (_food <= _population)
    {
        _population = _food;
        _food = 0;
    }
    else
    {
        _food -= _population;
    }

    if (_population == 0)
        kill();
}
