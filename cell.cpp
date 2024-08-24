#include "cell.h"
#include "worldsimulation.h"

#include <QRandomGenerator>

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
        int energyToTransfer = m_energy * DigitalEvolution::TRANSPORT_ENERGY_PROPORTION;
        m_energy -= energyToTransfer;
        energyToTransfer /= m_routingTable.weightsSum();
        if (energyToTransfer < 0)
            throw std::runtime_error("negative energyToTransfer");

        Cell* cell = nullptr;
        for (int i = static_cast<int>(Direction::Left); i <= static_cast<int>(Direction::Down); ++i)
        {
            if (m_routingTable.weight(static_cast<Direction>(i)) != 0)
            {
                cell = world.getCell(world.getNeighborPos(m_x, m_y, static_cast<Direction>(i)));
                if (cell)
                    cell->addEnergyToBuffer(energyToTransfer, world.stepsNumber());
                else
                    throw std::runtime_error("nullptr cell in transportEnergy");
            }
        }
    }
}

void Cell::die(WorldSimulation &world)
{
    Cell* cell = nullptr;

    for (int i = static_cast<int>(Direction::Left); i <= static_cast<int>(Direction::Down); ++i)
    {
        Direction direction = static_cast<Direction>(i);
        cell = world.getCell(world.getNeighborPos(m_x, m_y, direction));
        if (cell) cell->onNeighborDied(mirrorDirection(direction));
    }

    m_isDead = true;
    world.preEraseCell(m_x, m_y);
}

void Cell::onNeighborDied(Direction neighborDirection)
{
    if (m_routingTable.parentDirection() == neighborDirection)
        m_routingTable.setParentDirection(Direction::None);
    m_routingTable.resetWeight(neighborDirection);
}

/******************************************************************************/

Leaf::Leaf(size_t x, size_t y, size_t energy, Direction parent)
    : Cell(x, y, energy)
{
    m_color = 0xff32CD32;
    m_routingTable.setTransportPolicy(TransportPolicy::Source);
    m_routingTable.setParentDirection(parent);
    m_energyNeed = DigitalEvolution::ENERGY_NEED;
}

void Leaf::act(WorldSimulation &world)
{
    m_energy += DigitalEvolution::ENERGY_GENERATE;
}

/******************************************************************************/

Sprout::Sprout(size_t x, size_t y, size_t energy)
    : Cell(x, y, energy)
{
    m_color = 0xffFFAACC;
    m_routingTable.setTransportPolicy(TransportPolicy::Сonsumer);
    m_energyNeed = DigitalEvolution::ENERGY_NEED;
    for (size_t i = 0; i < m_genom.size(); ++i)
        m_genom[i] = QRandomGenerator::global()->bounded(256);
    m_direction = static_cast<Direction>(
                QRandomGenerator::global()->bounded(static_cast<int>(Direction::Left), static_cast<int>(Direction::Down)));

}

Sprout::Sprout(size_t x, size_t y, size_t energy, Direction direction, Direction parent,
               size_t activeGen, std::array<uint8_t, 9> genom)
    : Cell(x, y, energy)
{
    m_color = 0xffFFAACC;
    m_routingTable.setTransportPolicy(TransportPolicy::Сonsumer);
    m_routingTable.setParentDirection(parent);
    m_energyNeed = DigitalEvolution::ENERGY_NEED;
    m_direction = direction;
    m_activeGen = activeGen;
    m_genom = genom;
    if (QRandomGenerator::global()->bounded(20) == 0)
        m_genom[QRandomGenerator::global()->bounded(m_genom.size())] = QRandomGenerator::global()->bounded(256);
}

void Sprout::act(WorldSimulation &world)
{
    int energyNeedSum = m_energyNeed;
    int childCount = 0;
    Direction directions[3];
    QPoint points[3];
    if (m_direction == Direction::Left)
    {
        directions[0] = Direction::Down;
        points[0] = world.getDownPos(m_x, m_y);
        directions[1] = Direction::Left;
        points[1] = world.getLeftPos(m_x, m_y);
        directions[2] = Direction::Up;
        points[2] = world.getUpPos(m_x, m_y);
    }
    else if (m_direction == Direction::Up)
    {
        directions[0] = Direction::Left;
        points[0] = world.getLeftPos(m_x, m_y);
        directions[1] = Direction::Up;
        points[1] = world.getUpPos(m_x, m_y);
        directions[2] = Direction::Right;
        points[2] = world.getRightPos(m_x, m_y);
    }
    else if (m_direction == Direction::Right)
    {
        directions[0] = Direction::Up;
        points[0] = world.getUpPos(m_x, m_y);
        directions[1] = Direction::Right;
        points[1] = world.getRightPos(m_x, m_y);
        directions[2] = Direction::Down;
        points[2] = world.getDownPos(m_x, m_y);
    }
    else if (m_direction == Direction::Down)
    {
        directions[0] = Direction::Right;
        points[0] = world.getRightPos(m_x, m_y);
        directions[1] = Direction::Down;
        points[1] = world.getDownPos(m_x, m_y);
        directions[2] = Direction::Left;
        points[2] = world.getLeftPos(m_x, m_y);
    }
    else
        throw std::runtime_error("unidentified m_direction");

    for (int i = m_activeGen * 3; i < m_activeGen * 3 + 3; ++i)
        if (m_genom[i] < 120)
        {
            if (world.getCell(points[i - m_activeGen * 3]) == nullptr)
            {
                energyNeedSum += DigitalEvolution::ENERGY_NEED_TO_BORN;
                ++childCount;
            }
        }
    if (childCount > 0 && m_energy >= energyNeedSum)
    {
        m_energy -= energyNeedSum;
        int energyToChild = m_energy / (childCount + 1);

        m_isDead = true;
        world.preEraseCell(m_x, m_y);
        Cell* newMe = world.insertCellBeforeCur(std::make_unique<Transport>(m_x, m_y, energyToChild, m_routingTable.parentDirection()));
        if (!newMe)
            throw std::runtime_error("nullptr newMe");
        for (int i = m_activeGen * 3; i < m_activeGen * 3 + 3; ++i)
        {
            if (world.getCell(points[i - m_activeGen * 3]) == nullptr)
            {
                int x = points[i - m_activeGen * 3].x();
                int y = points[i - m_activeGen * 3].y();
                Direction childDirection = directions[i - m_activeGen * 3];
                Direction myDirectionForChild = mirrorDirection(childDirection);

                Cell* child = nullptr;
                if (m_genom[i] < 60)
                {
                    child = world.insertCellBeforeCur(std::make_unique<Sprout>(x, y, energyToChild,
                                                                               childDirection, myDirectionForChild,
                                                                               m_genom[i] % kGenCount, m_genom));
                    if (!child)
                        throw std::runtime_error("nullptr child");
                    newMe->setWeight(childDirection, 1);
                }
                else if (m_genom[i] < 120)
                {
                    child = world.insertCellBeforeCur(std::make_unique<Leaf>(x, y, energyToChild, myDirectionForChild));
                    if (!child)
                        throw std::runtime_error("nullptr child");
                    child->setWeight(myDirectionForChild, 1);
                }
            }
        }
    }
}

/******************************************************************************/

Transport::Transport(size_t x, size_t y, size_t energy, Direction parent)
    : Cell(x, y, energy)
{
    m_color = 0xffb3b3b3;
    m_routingTable.setTransportPolicy(TransportPolicy::Transporter);
    m_energyNeed = DigitalEvolution::ENERGY_NEED;
    m_routingTable.setParentDirection(parent);
}

void Transport::act(WorldSimulation &world)
{
}

}
