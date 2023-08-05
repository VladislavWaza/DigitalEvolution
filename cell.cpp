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
        _genom[i] %= (_maxGene + 1);
    _food = 1;
    _strength = 1;
    _direction = Clan::_directions[0];
    _isAlive = true;
    _color = Qt::darkCyan;
}

Clan::Clan(const Clan &parent)
{
    _genom = new uint8_t[_size];
    for (int i = 0; i < _size; ++i)
        _genom[i] = parent._genom[i];
    if (QRandomGenerator::system()->bounded(100) < Clan::_percentMutation)
       _genom[QRandomGenerator::system()->bounded(_size)] = QRandomGenerator::system()->bounded(_maxGene + 1);
    _food = 1;
    _strength = 1;
    _direction = Clan::_directions[0];
    _isAlive = true;
    _color = Qt::darkCyan;
}

Clan::~Clan()
{
}

QColor Clan::getColor()
{
    return _color;
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

int Clan::getStrength()
{
    return _strength;
}

void Clan::setStrength(int strength)
{
    if (strength >= 1 && strength <= _maxStrength)
        _strength = strength;
}

int Clan::getFood()
{
    return _food;
}

void Clan::increaseFood(int food)
{
    _food += food;
    if (_food > _maxFood)
        _food = _maxFood;
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
    _food -= _strength;
    if (_food < 0)
        kill();
}
