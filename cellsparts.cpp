#include "cellsparts.h"
#include "worldsimulation.h"

namespace DigitalEvolution
{

int EnergyBuffer::pop(size_t curStep)
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

void EnergyBuffer::push(size_t curStep, int energy)
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
    if (m_weightsSum == 0 && m_parentDirection == Direction::None &&
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
    if (m_weightsSum == 0 && m_parentDirection != Direction::None && m_transportPolicy != TransportPolicy::Consumer)
    {
        // передем энергию родителю
        setWeight(m_parentDirection, 1);

        // при этом надо родителю отменить передачу
        Cell* parent = world.getCell(world.getNeighborPos(x, y, m_parentDirection));

        if (parent && !parent->isDead())
            parent->m_routingTable.resetWeight(mirrorDirection(m_parentDirection));
        else
            throw std::runtime_error("parent nullptr or dead");
    }
}

void RoutingTable::resetWeight(Direction direction)
{
    m_weightsSum -= m_weights[static_cast<int>(direction)];
    m_weights[static_cast<int>(direction)] = 0;
}

void RoutingTable::setWeight(Direction direction, uint8_t weight)
{
    m_weightsSum += weight - m_weights[static_cast<int>(direction)];
    m_weights[static_cast<int>(direction)] = weight;
}

}
