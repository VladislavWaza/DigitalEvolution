#include "cell.h"
#include "worldsimulation.h"

Cell::Cell(size_t x, size_t y, size_t energy)
    : m_x(x), m_y(y), m_energy(energy)
{
}

void Cell::doAct(WorldSimulation &world)
{
    if (world.stepsNumber() > m_stepEnergyBufferUpdate)
    {
        m_energy += m_energyBuffer;
        m_energyBuffer = 0;
        m_stepEnergyBufferUpdate = 0;
    }
    act(world);
}

Leaf::Leaf(size_t x, size_t y, size_t energy)
    : Cell(x, y, energy)
{
    m_color = 0xff32CD32;
}

void Leaf::act(WorldSimulation &world)
{
}
