#include "cellsparts.h"

int EnergyBuffer::get(size_t curStep)
{
    int result = 0;
    if (curStep > m_stepEnergyBufferUpdate)
    {
        result = m_energyBuffer;
        m_energyBuffer = 0;
        m_stepEnergyBufferUpdate = 0;
    }
    return result;
}

void EnergyBuffer::add(size_t curStep, int energy)
{
    m_energyBuffer += energy;
    m_stepEnergyBufferUpdate = curStep;
}
