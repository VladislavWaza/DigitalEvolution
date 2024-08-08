#ifndef CELL_H
#define CELL_H

#include "stdlib.h"

#include <QColor>

class WorldSimulation;

/*
 * Передача энергии осуществляется через буфер:
 * - энергия кладется в буфер
 * - записывается номер итерации, на котором положили энергию
 * - в начале работы клетки, если номер текущей итерации больше итерации обновления буфера,
 *   то энергия переходит из буфера непосредсвтенно в клетку
 * - если энергия кладется в буфер и номер текущей итарации больше итерации обновления буфера,
 *   то энергия также переходит из буфера непосредсвтенно в клетку
 *
 */
class Cell
{
public:
    Cell(size_t x, size_t y, size_t energy = 0);
    virtual ~Cell() = default;

    QRgb color() const {return m_color;}

    void doAct(WorldSimulation& world);
    void addEnergyToBuffer(size_t energy, size_t curStepNumber);

protected:
    virtual void act(WorldSimulation& world) = 0;

    size_t m_x = 0;
    size_t m_y = 0;

    int m_energy = 0;
    int m_energyBuffer = 0;
    size_t m_stepEnergyBufferUpdate = 0;

    QRgb m_color = 0xffaaaaaa;
};

class Leaf : public Cell
{
public:
    Leaf(size_t x, size_t y, size_t energy = 0);
    ~Leaf() = default;

protected:
    void act(WorldSimulation& world) override;
};

#endif // CELL_H
