#ifndef CELL_H
#define CELL_H

#include "stdlib.h"

#include <QColor>

class WorldSimulation;

class Cell
{
public:
    Cell(size_t x, size_t y, size_t energy = 0);
    virtual ~Cell() = default;

    QRgb color() const {return m_color;}

    void doAct(WorldSimulation& world);

protected:
    virtual void act(WorldSimulation& world) = 0;

    size_t m_x = 0;
    size_t m_y = 0;

    int m_energy = 0;
    int m_energyBuffer = 0;
    size_t m_stepEnergyBufferUpdate = 0;

    QRgb m_color = 0xffaaaaaa;
};

class Leaf : public Cell
{
public:
    Leaf(size_t x, size_t y, size_t energy = 0);
    ~Leaf() = default;

protected:
    void act(WorldSimulation& world) override;
};

#endif // CELL_H
