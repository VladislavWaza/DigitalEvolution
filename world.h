#ifndef WORLD_H
#define WORLD_H

#include "clan.h"
#include "region.h"

/* Нужен чтобы отслеживать положение кланов и выполнять геном кланов
 */


class World
{
public:
    World(int squareSide, int w = 0, int h = 0);

    //Добавляет клан в мир
    void addItem(int x, int y, Clan *clan);

    //возвращает номера ячеек где еще нет клана
    //из этого номера можно вычислить положение ячейки
    //x = number / height
    //y = number % height
    void getNumsOfEmptySpaces(QList<int> &list);

    //последовательно выполняет геномы всех кланов, добавленных в мир
    void run();
private:
    QList<Clan*> _clans;
    int _w, _h, _squareSide;
};


#endif // WORLD_H
