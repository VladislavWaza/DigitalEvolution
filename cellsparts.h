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

    int get(size_t curStep);
    void add(size_t curStep, int energy);

private:
    int m_energyBuffer = 0;
    size_t m_stepEnergyBufferUpdate = 0;
};


class RoutingTable
{
public:
    RoutingTable() = default;
    RoutingTable(Direction parentDirection, TransportPolicy transportPolicy);

    void setTransportPolicy(TransportPolicy transportPolicy) {m_transportPolicy = transportPolicy;}
    int energyToSum() const {return m_energyToSum;}
    uint8_t energyTo(Direction direction) const {return m_energyTo[static_cast<int>(direction)];}

    bool shouldDie() const;
    void update(WorldSimulation& world, int x, int y);
    void onNeighborDied(Direction neighborDirection);
private:
    uint8_t m_energyTo[4] = {0, 0, 0, 0};
    int m_energyToSum = 0;
    Direction m_parentDirection = Direction::None;
    TransportPolicy m_transportPolicy = TransportPolicy::None;
};

}
#endif // CELLSPARTS_H
