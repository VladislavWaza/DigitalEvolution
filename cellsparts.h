#ifndef CELLSPARTS_H
#define CELLSPARTS_H

#include "params.h"

#include <stdint.h>

namespace DigitalEvolution
{

class WorldSimulation;
class Cell;

class EnergyBuffer
{
public:
    EnergyBuffer() = default;

    int getEnergy() const {return m_energyBuffer;}

    int pop(size_t curStep);
    void push(size_t curStep, int energy);

private:
    int m_energyBuffer = 0;
    size_t m_stepEnergyBufferUpdate = 0;
};


class RoutingTable
{
public:
    RoutingTable() = default;
    RoutingTable(Direction parentDirection, TransportPolicy transportPolicy);

    Direction parentDirection() const {return m_parentDirection;}
    void setParentDirection(Direction direction) {m_parentDirection = direction;}
    void setTransportPolicy(TransportPolicy transportPolicy) {m_transportPolicy = transportPolicy;}
    int weightsSum() const {return m_weightsSum;}
    uint8_t weight(Direction direction) const {return m_weights[static_cast<int>(direction)];}

    void resetWeight(Direction direction);
    void setWeight(Direction direction, uint8_t weight);

    bool shouldDie() const;
    void update(WorldSimulation& world, int x, int y);
private:
    uint8_t m_weights[4] = {0, 0, 0, 0};
    int m_weightsSum = 0;
    Direction m_parentDirection = Direction::None;
    TransportPolicy m_transportPolicy = TransportPolicy::None;
};

}
#endif // CELLSPARTS_H
