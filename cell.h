#ifndef CELL_H
#define CELL_H

#include "stdlib.h"

class WorldSimulation;

struct Cell
{
    size_t x = 0;
    size_t y = 0;

    Cell(size_t x, size_t y) : x(x), y(y) {}
    ~Cell() = default;
};

#endif // CELL_H
