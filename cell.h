#ifndef CELL_H
#define CELL_H

#include "stdlib.h"

#include <QColor>

namespace DigitalEvolution
{

const int ENERGY_NEED = 20;
const float TRANSPORT_ENERGY_PROPORTION = 1;// доля энергии к передаче


class WorldSimulation;

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
 * - Опицанальные веса m_energyTo
 *
 * - инкапсуляция в струтуры
 *
 * - Адреса соседей для отрисовки связей
 * - отрисвока энергий
 * - Номер клана
 * - несколько способов передачи энергии
 * - старость клеток
 * - ограничение числа делений
 * - статический генератор
 */
class Cell
{
public:

    enum class Direction : int {None = -1, Left = 0, Up = 1, Right = 2, Down = 3};
    enum class TransportPolicy {None, Сonsumer, Source, Transporter};

    Cell(size_t x, size_t y, size_t energy = 0);
    virtual ~Cell() = default;

    bool isDead() const {return m_isDead;}
    QRgb color() const {return m_color;}
    size_t x() const {return m_x;}
    size_t y() const {return m_y;}

    void doAct(WorldSimulation& world);

protected:
    void addEnergyToBuffer(size_t energy, size_t curStepNumber);
    void transportEnergy(WorldSimulation& world);
    void die(WorldSimulation &world);
    void yourNeighborDied(Direction neighborDirection);
    virtual void act(WorldSimulation& world) = 0;

    size_t m_x = 0;
    size_t m_y = 0;

    int m_energy = 0;
    int m_energyBuffer = 0;
    size_t m_stepEnergyBufferUpdate = 0;
    bool m_isDead = false;

    int m_energyNeed = 0;
    QRgb m_color = 0xffaaaaaa;

    uint8_t m_energyTo[4] = {0, 0, 0, 0};
    int m_energyToSum = 0;
    Direction m_parentDirection = Direction::None;
    TransportPolicy m_transportPolicy = TransportPolicy::None;
};

/******************************************************************************/

class Leaf : public Cell
{
public:
    Leaf(size_t x, size_t y, size_t energy = 0);
    ~Leaf() = default;

protected:
    void act(WorldSimulation& world) override;
};

/******************************************************************************/

class Transport : public Cell
{
public:
    Transport(size_t x, size_t y, size_t energy = 0);
    ~Transport() = default;

protected:
    void act(WorldSimulation& world) override;
};

/******************************************************************************/

class Sprout : public Cell
{
public:
    Sprout(size_t x, size_t y, size_t energy = 0);
    ~Sprout() = default;

protected:
    void act(WorldSimulation& world) override;
};

}

#endif // CELL_H
