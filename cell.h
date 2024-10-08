#ifndef CELL_H
#define CELL_H

#include "cellsparts.h"

#include <QColor>
#include <QRandomGenerator>

namespace DigitalEvolution
{

class WorldSimulation;
class Sprout;

/*
 * Общий алгоритм работы клетки:
 * 1. обновить буфер
 * 3. выполнять действия (делится, вырабатывать энергию)
 * 4. затратить энергию на жизнь
 * 5. проверить жив ли
 * 6. обновить правила транcпортировки
 * 7. передать энергию (всю что есть)
 *
 * Алгоритм генома и ростка:
 * 1. Хватает энергии? -Делай то что ниже
 * 2. Удаляй себя, но не говори соседям
 * 3. Вместо себя добавь транспорную
 * 4. Рожай
 *
 *
 * Смерть клетки в общем виде:
 * - если сейчас текущая итерация клетки, то она без проблем умирает, и её тут же очищает класс симуляции
 * - если клетка умирает или её убивают, то в m_cells надо поставить nullptr и тогда
 *   для всех последующих итераций клетки сущестовать не будет (предудаление)
 * - обрататывать смерть нужно сразу, не дожидаясь своей итерации
 *
 * - вообще с атакой на другие клетки все неоднозначно, потому то вмешивается порядок хода и буфер
 *
 * Передача энергии осуществляется через буфер:
 * - энергия кладется в буфер
 * - записывается номер итерации, на котором положили энергию
 * - в начале работы клетки, если номер текущей итерации больше итерации обновления буфера,
 *   то энергия переходит из буфера непосредсвтенно в клетку
 * - если энергия кладется в буфер и номер текущей итарации больше итерации обновления буфера,
 *   то энергия также переходит из буфера непосредсвтенно в клетку
 *
 * Транспортировка энергии описывается в
 * m_energyTo (m_energyTo[Direction] != 0 => надо передать энергию по направлению Direction)
 * и m_parentDirection и TransportPolicy:
 * - Все клетки обладают алгоритмом транспортировки
 * - Есть три типа клеток
 *    1. те кто дают (Листья и другие добытчики энергии)
 *    2. те кто передают (Стебли)
 *    3. те кто получают (Ростки)
 *
 * - Росток создает новую клетку:
 *    1. Устанавливает их родителем себя
 *    2. Если тип новой клетки подразумевает получение энергии, то росток
 *       устанавливает себе это в m_energyTo
 *    3. Если тип новой клетки подразумевает генерацию энергии, то росток
 *       устанавливает клетке это в m_energyTo
 * - Смерть клетки:
 *    1. Обратиться ко всем соседям и у детей отменить свое родительство
 *    2. Обратиться ко всем соседям и сообщить им, что энергию больше передвать не надо
 *    3. Умереть
 * - Перенаправление потоков энергии:
 *    1. Если передавать энергию некуда, то передавать родителю и отменить передачу от него
 *    2. Если родителя нет, то помереть
 *    ? когда происходит перенаправление? сразу по цепочке или только в свою итерацию (в свою)
 *
 * - Опициональное подключение времени жизни:
 *    1. При рождении клетка проверяет наличие клеток рядом кроме родителя
 *    2. При смерти все как обычно
 *    3. Если есть кому передвать энергию, то передает
 *    4. Если есть от кого получать энергию, то просим передать
 *    ? Если подключающаяся клетка - стебель и целевая тоже, то двухстороняя передача
 *
 *
 * - Адреса соседей для отрисовки связей (адреса не нужны, достаточно направления)
 * - смена режима в паузе и при добавлении клеток
 * - Номер клана
 * - несколько способов передачи энергии
 * - старость клеток
 * - ограничение числа делений
 * - адектватно сделать атаку
 */

class Cell
{
    friend void RoutingTable::update(WorldSimulation &world, int x, int y);
public:
    Cell(size_t x, size_t y, size_t energy);
    Cell(size_t x, size_t y, size_t energy, Direction parent);
    virtual ~Cell() = default;

    bool isDead() const {return m_isDead;}
    QRgb color() const {return m_color;}
    int energy() const {return m_energy;}
    int allEnergy() const {return m_energy + m_energyBuffer.getEnergy();}
    int bufferEnergy() const {return m_energyBuffer.getEnergy();}
    size_t x() const {return m_x;}
    size_t y() const {return m_y;}
    size_t age() const {return m_age;}
    Direction parentDirection() const {return m_routingTable.parentDirection();}
    EnergyBuffer energyBuffer() const {return m_energyBuffer;}

    void doAct(WorldSimulation& world);

    void setWeight(Direction direction, uint8_t weight) {m_routingTable.setWeight(direction, weight);}

protected:
    static QRandomGenerator *s_generator;

    void addEnergyToBuffer(int energy, size_t curStepNumber);
    void transportEnergy(WorldSimulation& world);
    void die(WorldSimulation &world);
    void onNeighborDied(Direction neighborDirection);
    virtual void act(WorldSimulation& world) = 0;

    size_t m_x = 0;
    size_t m_y = 0;
    size_t m_age = 0;

    bool m_isDead = false;

    int m_energy = 0;
    int m_energyNeed = 0;
    EnergyBuffer m_energyBuffer;
    RoutingTable m_routingTable;

    QRgb m_color = 0xffaaaaaa;
};

/******************************************************************************/

class Leaf : public Cell
{
public:
    Leaf(size_t x, size_t y, size_t energy, Direction parent);
    ~Leaf() = default;

protected:
    void act(WorldSimulation& world) override;
};

/******************************************************************************/

class Transport : public Cell
{
public:
    Transport(const Sprout& sprout, size_t newEnergy);
    ~Transport() = default;

protected:
    void act(WorldSimulation& world) override;
};

/******************************************************************************/

class Sprout : public Cell
{
public:
    Sprout(size_t x, size_t y, size_t energy);
    Sprout(size_t x, size_t y, size_t energy, Direction direction, Direction parent,
           size_t activeGen, std::array<uint8_t, 9> genom);
    ~Sprout() = default;

protected:
    void act(WorldSimulation& world) override;

    std::array<uint8_t, 9> m_genom;
    const int kGenCount = 3;

    int m_activeGen = 0;
    Direction m_direction = Direction::None;
};

}

#endif // CELL_H
