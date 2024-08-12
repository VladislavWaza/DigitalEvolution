#ifndef CELLSPARTS_H
#define CELLSPARTS_H

#include <stddef.h>

class EnergyBuffer
{
public:
    EnergyBuffer() = default;

    int get(size_t curStep);
    void add(size_t curStep, int energy);

private:
    int m_energyBuffer = 0;
    size_t m_stepEnergyBufferUpdate = 0;
};


#endif // CELLSPARTS_H
