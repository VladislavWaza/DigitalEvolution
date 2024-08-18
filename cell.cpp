#include "cell.h"
#include "worldsimulation.h"

namespace DigitalEvolution
{

Cell::Cell(size_t x, size_t y, size_t energy)
    : m_x(x), m_y(y), m_energy(energy)
{
}

void Cell::doAct(WorldSimulation &world)
{
    if (m_isDead)
        return;

    // Обновляем буфер энергии
    m_energy += m_energyBuffer.get(world.stepsNumber());

    // Выполняем действия
    act(world);
    if (m_isDead) return;

    // Тратим энергию на жизнь
    m_energy -= m_energyNeed;

    // Проверяем живы ли
    if (m_energy < 0)
    {
        die(world);
        return;
    }

    // Передаем энергию
    transportEnergy(world);
    // if (m_isDead) return;
}

void Cell::addEnergyToBuffer(int energy, size_t curStepNumber)
{
    // сбрасываем буфер если он еще не сброшен
    m_energy += m_energyBuffer.get(curStepNumber);
    // добавляем
    m_energyBuffer.add(curStepNumber, energy);
}

void Cell::transportEnergy(WorldSimulation &world)
{
    if (m_routingTable.shouldDie())
    {
        die(world);
        return;
    }
    m_routingTable.update(world, m_x, m_y);

    // Если есть куда передавать энергию, то передаем
    if (m_routingTable.weightsSum() != 0)
    {
        int energyToTransfer = m_energy / m_routingTable.weightsSum() * DigitalEvolution::TRANSPORT_ENERGY_PROPORTION;
        if (energyToTransfer < 0)
            throw std::runtime_error("negative energyToTransfer");
        m_energy = 0;

        Cell* cell = nullptr;
        for (int i = static_cast<int>(Direction::Left); i <= static_cast<int>(Direction::Down); ++i)
        {
            if (m_routingTable.weight(static_cast<Direction>(i)) != 0)
            {
                cell = world.getCell(world.getNeighborPos(m_x, m_y, static_cast<Direction>(i)));
                if (cell)
                    cell->addEnergyToBuffer(energyToTransfer, world.stepsNumber());
            }
        }
    }
}

void Cell::die(WorldSimulation &world)
{
    Cell* cell = nullptr;
    cell = world.getCell(world.getLeftPos(m_x, m_y));
    if (cell)
        cell->onNeighborDied(Direction::Right);

    cell = world.getCell(world.getRightPos(m_x, m_y));
    if (cell)
        cell->onNeighborDied(Direction::Left);

    cell = world.getCell(world.getUpPos(m_x, m_y));
    if (cell)
        cell->onNeighborDied(Direction::Down);

    cell = world.getCell(world.getDownPos(m_x, m_y));
    if (cell)
        cell->onNeighborDied(Direction::Up);

    m_isDead = true;
    world.preEraseCell(m_x, m_y);
}

void Cell::onNeighborDied(Direction neighborDirection)
{
    if (neighborDirection == Direction::None)
        throw std::runtime_error("unidentified neighborDirection");
    if (m_routingTable.parentDirection() == neighborDirection)
        m_routingTable.setParentDirection(Direction::None);
    m_routingTable.resetWeight(neighborDirection);
}

/******************************************************************************/

Leaf::Leaf(size_t x, size_t y, size_t energy)
    : Cell(x, y, energy)
{
    m_color = 0xff32CD32;
    m_routingTable.setTransportPolicy(TransportPolicy::Source);
    m_energyNeed = DigitalEvolution::ENERGY_NEED;
}

void Leaf::act(WorldSimulation &world)
{
}

/******************************************************************************/

Sprout::Sprout(size_t x, size_t y, size_t energy)
    : Cell(x, y, energy)
{
    m_color = 0xffFFAACC;
    m_routingTable.setTransportPolicy(TransportPolicy::Сonsumer);
    m_energyNeed = DigitalEvolution::ENERGY_NEED;
}

void Sprout::act(WorldSimulation &world)
{
}

/******************************************************************************/

Transport::Transport(size_t x, size_t y, size_t energy)
    : Cell(x, y, energy)
{
    m_color = 0xffb3b3b3;
    m_routingTable.setTransportPolicy(TransportPolicy::Transporter);
    m_energyNeed = DigitalEvolution::ENERGY_NEED;
}

void Transport::act(WorldSimulation &world)
{
}

}
