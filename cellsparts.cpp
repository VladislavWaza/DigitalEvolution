#include "cellsparts.h"
#include "worldsimulation.h"

namespace DigitalEvolution
{

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

RoutingTable::RoutingTable(Direction parentDirection, TransportPolicy transportPolicy)
    : m_parentDirection(parentDirection), m_transportPolicy(transportPolicy)
{
}

bool RoutingTable::shouldDie() const
{
    // Если передвать некуда и родителя нет
    if (m_energyToSum == 0 && m_parentDirection == Direction::None &&
            //Если клетка транспортая или клетка-источник, то умереть так как смысл жизни потерян
            (m_transportPolicy == TransportPolicy::Transporter || m_transportPolicy == TransportPolicy::Source))
    {
        return true;
    }
    return false;
}

void RoutingTable::update(WorldSimulation &world, int x, int y)
{
    // Eсли передвать некуда и родитель есть и клетка сама не потребитель
    if (m_energyToSum == 0 && m_parentDirection != Direction::None && m_transportPolicy != TransportPolicy::Сonsumer)
    {
        // передем энергию родителю
        m_energyTo[static_cast<int>(m_parentDirection)] = 1;
        ++m_energyToSum;

        // при этом надо родителю отменить передачу
        Direction myDir = mirrorDirection(m_parentDirection);
        Cell* parent = world.getCell(world.getNeighborPos(x, y, m_parentDirection));
        if (!parent) throw std::runtime_error("parent nullptr");
        if (parent->isDead()) throw std::runtime_error("parent dead");

        RoutingTable& parentRoutingTable = parent->m_routingTable;

        if (parentRoutingTable.m_energyTo[static_cast<int>(myDir)] != 0)
        {
            parentRoutingTable.m_energyTo[static_cast<int>(myDir)] = 0;
            --(parentRoutingTable.m_energyToSum);
        }
    }
}

void RoutingTable::onNeighborDied(Direction neighborDirection)
{
    if (m_parentDirection == neighborDirection)
        m_parentDirection = Direction::None;
    if (m_energyTo[static_cast<int>(neighborDirection)] != 0)
    {
        m_energyTo[static_cast<int>(neighborDirection)] = 0;
        --m_energyToSum;
    }
}

}
