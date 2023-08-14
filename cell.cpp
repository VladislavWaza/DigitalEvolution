#include <QRandomGenerator>
#include "cell.h"

QPoint const Cell::_directions[] = {QPoint(-1, 0), QPoint(-1, -1), QPoint(0, -1),
                                QPoint(1, -1), QPoint(1, 0), QPoint(1, 1), QPoint(0, 1), QPoint(-1, 1)};

Cell::Cell(GenomeInitType genomeInitType)
    :QObject()
{
    _genom = new uint8_t[_size];
    if (genomeInitType == GenomeInitType::Random)
    {
        for (int i = 0; i < _size/4; ++i)
            *reinterpret_cast<uint32_t*>(&_genom[4*i]) = QRandomGenerator::system()->generate();
        for (int i = 0; i < _size; ++i)
            _genom[i] %= (_maxGene + 1);
    }
    if (genomeInitType == GenomeInitType::Collector)
    {
        for (int i = 0; i < _size; ++i)
            _genom[i] = 10;
    }
    _food = 1;
    _strength = 1;
    _direction = Cell::_directions[0];
    _isAlive = true;
    _color = Qt::darkCyan;
    _age = 0;
}

Cell::Cell(const Cell &parent)
    :QObject()
{
    _genom = new uint8_t[_size];
    for (int i = 0; i < _size; ++i)
        _genom[i] = parent._genom[i];
    if (QRandomGenerator::system()->bounded(100) < Cell::_percentMutation)
       _genom[QRandomGenerator::system()->bounded(_size)] = QRandomGenerator::system()->bounded(_maxGene + 1);
    _food = 1;
    _strength = 1;
    _direction = Cell::_directions[0];
    _isAlive = true;
    _color = Qt::darkCyan;
    _age = 0;
}

Cell::~Cell()
{
}

QColor Cell::getColor()
{
    return _color;
}

void Cell::getGenom(uint8_t *ptr) const
{
    for (int i = 0; i < _size; ++i)
        ptr[i] = _genom[i];
}

int Cell::calculateGenomesDiff(const Cell &other) const
{
    int c = 0;
    for (int i = 0; i < Cell::_size; ++i)
    {
        if (_genom[i] != other._genom[i])
            ++c;
    }
    return c;
}

void Cell::setDirection(QPoint direction)
{
    _direction = direction;
}

QPoint Cell::getDirection()
{
    return _direction;
}

int Cell::getStrength()
{
    return _strength;
}

void Cell::setStrength(int strength)
{
    if (strength >= 1 && strength <= _maxStrength)
        _strength = strength;
}

float Cell::getFood()
{
    return _food;
}

void Cell::increaseFood(float food)
{
    _food += food;
    if (_food > _maxFood)
        _food = _maxFood;
}

int Cell::getAge()
{
    return _age;
}

bool Cell::isAlive()
{
    return _isAlive;
}

void Cell::kill()
{
    _isAlive = false;
    emit signalKilled();
}

void Cell::survive()
{
    ++_age;

    _food -= _strength;
    if (_food <= 0 || _age > _maxAge)
        kill();
}
