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
    // Обновляем буфер энергии
    if (world.stepsNumber() > m_stepEnergyBufferUpdate)
    {
        m_energy += m_energyBuffer;
        m_energyBuffer = 0;
        m_stepEnergyBufferUpdate = 0;
    }

    // Выполняем действия
    act(world);

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
    if (m_isDead)
        return;
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

void Cell::transportEnergy(WorldSimulation &world)
{
    if (m_transportPolicy == TransportPolicy::None)
        throw std::runtime_error("unidentified TransportPolicy");

    // Есть куда передавать энергию?
    int energyToCount = 0;
    for (int i = static_cast<int>(Direction::Left); i <= static_cast<int>(Direction::Down); ++i)
    {
        if (m_energyTo[i] != 0)
            energyToCount += 1;
    }

    // Если передвать некуда и родителя нет
    if (energyToCount == 0 && m_parentDirection == Direction::None)
    {
        //Если клетка транспортая или клетка-источник, то умереть так как смысл жизни потерян
        if (m_transportPolicy == TransportPolicy::Transporter || m_transportPolicy == TransportPolicy::Source)
        {
            die(world);
            return;
        }
    }
    // Eсли передвать некуда и родитель есть и это не потребитель
    else if (energyToCount == 0 && m_parentDirection != Direction::None && m_transportPolicy != TransportPolicy::Сonsumer)
    {
        // передем энергию родителю
        m_energyTo[static_cast<int>(m_parentDirection)] = 1;
        ++energyToCount;
    }

    // Если есть куда передавать энергию, то передаем
    if (energyToCount != 0)
    {
        int energyToTransfer = m_energy / energyToCount * TRANSPORT_ENERGY_PROPORTION;
        if (energyToTransfer < 0)
            throw std::runtime_error("negative energyToTransfer");
        m_energy = 0;

        Cell* cell = nullptr;
        if (m_energyTo[static_cast<int>(Direction::Left)] != 0)
        {
            cell = world.getCell(world.getLeftPos(m_x, m_y));
            if (cell)
                cell->addEnergyToBuffer(energyToTransfer, world.stepsNumber());
        }

        if (m_energyTo[static_cast<int>(Direction::Up)] != 0)
        {
            cell = world.getCell(world.getUpPos(m_x, m_y));
            if (cell)
                cell->addEnergyToBuffer(energyToTransfer, world.stepsNumber());
        }

        if (m_energyTo[static_cast<int>(Direction::Right)] != 0)
        {
            cell = world.getCell(world.getRightPos(m_x, m_y));
            if (cell)
                cell->addEnergyToBuffer(energyToTransfer, world.stepsNumber());
        }

        if (m_energyTo[static_cast<int>(Direction::Down)] != 0)
        {
            cell = world.getCell(world.getDownPos(m_x, m_y));
            if (cell)
                cell->addEnergyToBuffer(energyToTransfer, world.stepsNumber());
        }
    }
}

void Cell::die(WorldSimulation &world)
{
    Cell* cell = nullptr;
    cell = world.getCell(world.getLeftPos(m_x, m_y));
    if (cell)
        cell->yourNeighborDied(Direction::Right);

    cell = world.getCell(world.getRightPos(m_x, m_y));
    if (cell)
        cell->yourNeighborDied(Direction::Left);

    cell = world.getCell(world.getUpPos(m_x, m_y));
    if (cell)
        cell->yourNeighborDied(Direction::Down);

    cell = world.getCell(world.getDownPos(m_x, m_y));
    if (cell)
        cell->yourNeighborDied(Direction::Up);

    m_isDead = true;
}

void Cell::yourNeighborDied(Direction neighborDirection)
{
    if (neighborDirection == Direction::None)
        throw std::runtime_error("unidentified neighborDirection");
    if (m_parentDirection == neighborDirection)
        m_parentDirection = Direction::None;
    m_energyTo[static_cast<int>(neighborDirection)] = 0;
}

/******************************************************************************/

Leaf::Leaf(size_t x, size_t y, size_t energy)
    : Cell(x, y, energy)
{
    m_color = 0xff32CD32;
    m_transportPolicy = TransportPolicy::Source;
    m_energyNeed = ENERGY_NEED;
}

void Leaf::act(WorldSimulation &world)
{
}

/******************************************************************************/

Sprout::Sprout(size_t x, size_t y, size_t energy)
    : Cell(x, y, energy)
{
    m_color = 0xffFFAACC;
    m_transportPolicy = TransportPolicy::Сonsumer;
    m_energyNeed = ENERGY_NEED;
}

void Sprout::act(WorldSimulation &world)
{
}

/******************************************************************************/

Transport::Transport(size_t x, size_t y, size_t energy)
    : Cell(x, y, energy)
{
    m_color = 0xffb3b3b3;
    m_transportPolicy = TransportPolicy::Transporter;
    m_energyNeed = ENERGY_NEED;
}

void Transport::act(WorldSimulation &world)
{

}

}
