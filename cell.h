#ifndef CELL_H
#define CELL_H

#include "stdlib.h"

#include <QColor>

class WorldSimulation;

struct Cell
{
    size_t x = 0;
    size_t y = 0;
    QRgb color = 0xffaaaaaa;

    Cell(size_t x, size_t y) : x(x), y(y) {}
    virtual ~Cell() = default;

    virtual void act(WorldSimulation& world) = 0;
};

struct Leaf : public Cell
{
    Leaf(size_t x, size_t y) : Cell(x, y) {}
    ~Leaf() = default;

    void act(WorldSimulation& world) override;
};

#endif // CELL_H
