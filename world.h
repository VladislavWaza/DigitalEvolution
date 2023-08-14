#ifndef WORLD_H
#define WORLD_H

#include <QImage>
#include "cell.h"
#include "region.h"

/* Нужен чтобы забрать задачу работы с миром у MainWindow
 *
 * Отслеживает положения клеток(class Cell) в мире, храня указатели на клетки
 * Выполняет их геномы
 * Автоматически удаляет клетки и регионы при удалении мира
 */


class World
{
public:
    static const QPoint CellUndefined;
    enum class DisplayMode{Сommon, Food, Strength, Age};

    World(int w = 0, int h = 0);
    ~World();

    int width();
    int height();

    /*изображение с клетками на прозрачном фоне в заданном режиме отображения*/
    void getCellsImage(QImage &img, DisplayMode mode);

    /*изображение с регионами на прозрачном фоне в заданном режиме отображения*/
    void getRegionsImage(QImage &img, DisplayMode mode);

    /*Возвращает указатель на клетку по указанным координатам
    * при координатах выходящих за границы мира или при отсутсвии клетки возвращает nullptr
    */
    Cell *getCell(int x, int y);

    /*Возвращает координаты клетки в мире, при отсутвии клетки возвращает World::CellUndefined = QPoint(-1,-1)*/
    QPoint getCellPos(Cell *cell);

    /*Возвращает указатель на регион по указанным координатам
    * при координатах выходящих за границы мира возвращает nullptr
    */
    Region *getRegion(int x, int y);


    /* Добавляет клетку в мир и возвращает true
     * при попытке добавления за пределы мира возращает false
     * при попытке добавления в уже занятый регион возвращает false
     */
    bool addCell(int x, int y, Cell *cell);

    /* Добавляет регион в мир и возвращает true
     * при попытке добавления в уже занятый регион заменяет, а старый удаляет
     * при попытке добавления за пределы мира возращает false
     */
    bool setRegion(int x, int y, Region *region);

    /* возвращает номера ячеек где еще нет клетки
     * из этого номера можно вычислить положение ячейки
     * x = number / height
     * y = number % height
     */
    void getNumsOfEmptySpaces(QList<int> &list);

    /* возвращает случайное место рядом с pos на расстоянии distance,
     * если до достижения distance было встречено препятсвие, то будет учитываться место до препятствия
     * если мест нет, то возвращает World::CellUndefined = QPoint(-1,-1)
     */
    QPoint randomEmptySpaceNearby(QPoint pos, int distance);

    /* возвращает число живых клеток*/
    int cellsNumber();

    /* последовательно выполняет геномы всех клеток, добавленных в мир
     * при несоответсвии размеров img и размеров мира возвращает false
     * требуется img такого формата, где клетке соответсвут один пиксель
     *
     * Алгоритм работы мира:
     * Для каждой клетки по очереди
     * 1.Проиходим по геному и выполняем команды
     * 2.Есть команды повернуть, атаковать, собрать еду, перейти
     * 3.Команды собрать еду, атаковать и перейти - завершающие
     * 4.Но если не было кого атаковать или некуда перейти, то не завершающие
     * 5.Проверка выживает ли клетка
     */
    void run();

private:
    QList<Cell*> _cells;
    QList<Region*> _regions;
    int _w, _h;


    //если это возможно, то передвинуть клетку вперед согласно её направлению, иначе false
    bool move(QPoint *pos, Cell *cell);

    //собрать еду согласно позиции в мире
    void collectFood(QPoint pos, Cell *cell);

    //если это возможно, то убить клетку по направлению поворота и забрать её еду, иначе false
    bool attack(QPoint *pos, Cell *cell);

    //создать дочернюю клетку если это возможно, pos - позиция родителя, dist - желаемое расстояние до дочерней клетки
    bool born(QPoint pos, Cell *cell, int dist);

    //повышает или понижает силу на 1 по направлению к цели, снимает за это 50 еды
    void aimStrength(int target, Cell *cell);

    //при выходе pos за границы мира, возвращает на место так будто мир закольцован
    void returnPosToWorld(QPoint *pos);

    //возвращает true если по направленю взгляда клетки пусто, иначе false
    bool isEmptyAhead(QPoint pos, Cell *cell);

    //возвращает true если по направленю взгляда клетки есть клетка и различие в их геноме не более 2 генов, иначе false
    bool isRelativeAhead(QPoint pos, Cell *cell);
};


#endif // WORLD_H
