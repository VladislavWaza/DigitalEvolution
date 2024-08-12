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
    if (m_transportPolicy == TransportPolicy::None)
        throw std::runtime_error("unidentified TransportPolicy");

    // Если передвать некуда и родителя нет
    if (m_energyToSum == 0 && m_parentDirection == Direction::None)
    {
        //Если клетка транспортая или клетка-источник, то умереть так как смысл жизни потерян
        if (m_transportPolicy == TransportPolicy::Transporter || m_transportPolicy == TransportPolicy::Source)
        {
            die(world);
            return;
        }
    }
    // Eсли передвать некуда и родитель есть и это не потребитель
    else if (m_energyToSum == 0 && m_parentDirection != Direction::None && m_transportPolicy != TransportPolicy::Сonsumer)
    {
        // передем энергию родителю
        m_energyTo[static_cast<int>(m_parentDirection)] = 1;
        ++m_energyToSum;

        Cell* parent = nullptr;
        Direction myDir = Direction::None;
        // при этом надо родителю отменить передачу
        if (m_parentDirection == Direction::Left)
        {
            myDir = Direction::Right;
            parent = world.getCell(world.getLeftPos(m_x, m_y));
        }
        else if (m_parentDirection == Direction::Up)
        {
            myDir = Direction::Down;
            parent = world.getCell(world.getUpPos(m_x, m_y));
        }
        else if (m_parentDirection == Direction::Down)
        {
            myDir = Direction::Up;
            parent = world.getCell(world.getDownPos(m_x, m_y));
        }
        else if (m_parentDirection == Direction::Right)
        {
            myDir = Direction::Left;
            parent = world.getCell(world.getRightPos(m_x, m_y));
        }
        if (!parent) throw std::runtime_error("parent nullptr");
        if (parent->isDead()) throw std::runtime_error("parent dead");
        if (parent->m_energyTo[static_cast<int>(myDir)] != 0)
        {
            parent->m_energyTo[static_cast<int>(myDir)] = 0;
            --(parent->m_energyToSum);
        }
    }

    // Если есть куда передавать энергию, то передаем
    if (m_energyToSum != 0)
    {
        int energyToTransfer = m_energy / m_energyToSum * DigitalEvolution::TRANSPORT_ENERGY_PROPORTION;
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
    world.preEraseCell(m_x, m_y);
}

void Cell::yourNeighborDied(Direction neighborDirection)
{
    if (neighborDirection == Direction::None)
        throw std::runtime_error("unidentified neighborDirection");
    if (m_parentDirection == neighborDirection)
        m_parentDirection = Direction::None;
    if (m_energyTo[static_cast<int>(neighborDirection)] != 0)
    {
        m_energyTo[static_cast<int>(neighborDirection)] = 0;
        --m_energyToSum;
    }
}

/******************************************************************************/

Leaf::Leaf(size_t x, size_t y, size_t energy)
    : Cell(x, y, energy)
{
    m_color = 0xff32CD32;
    m_transportPolicy = TransportPolicy::Source;
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
    m_transportPolicy = TransportPolicy::Сonsumer;
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
    m_transportPolicy = TransportPolicy::Transporter;
    m_energyNeed = DigitalEvolution::ENERGY_NEED;
}

void Transport::act(WorldSimulation &world)
{
}

}
