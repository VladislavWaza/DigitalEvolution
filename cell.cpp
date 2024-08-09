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

void Cell::addEnergyToBuffer(size_t energy, size_t curStepNumber)
{
    if (curStepNumber > m_stepEnergyBufferUpdate)
    {
        m_energy += m_energyBuffer;
        m_energyBuffer = energy;
        m_stepEnergyBufferUpdate = curStepNumber;
    }
    else if (curStepNumber == m_stepEnergyBufferUpdate)
    {
        m_energyBuffer += energy;
    }
    else
        throw std::runtime_error("curStepNumber less then stepEnergyBufferUpdate in Cell::addEnergyToBuffer");
}

Leaf::Leaf(size_t x, size_t y, size_t energy)
    : Cell(x, y, energy)
{
    m_color = 0xff32CD32;
}

void Leaf::act(WorldSimulation &world)
{
}

Sprout::Sprout(size_t x, size_t y, size_t energy)
    : Cell(x, y, energy)
{
    m_color = 0xffFFAACC;
}

void Sprout::act(WorldSimulation &world)
{
}
