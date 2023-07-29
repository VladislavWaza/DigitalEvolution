#ifndef WORLD_H
#define WORLD_H

#include <QImage>
#include "clan.h"
#include "region.h"

/* Нужен чтобы забрать задачу работы с миром у MainWinwow
 *
 * Отслеживает положения кланов(class Clan) в мире, храня указатели на кланы
 * Выполняет их геномы
 * Автоматически удаляет кланы при удалении мира
 */


class World
{
public:
    World(int w = 0, int h = 0);
    ~World();

    int width();
    int height();

    /* Добавляет клан в мир и возвращает true
     * при попытке добавления за пределы мира возращает false
     * При попытке добавления в уже занятый регион возвращает false
     */
    bool addClan(int x, int y, Clan *clan);

    /* возвращает номера ячеек где еще нет клана
     * из этого номера можно вычислить положение ячейки
     * x = number / height
     * y = number % height
     */
    void getNumsOfEmptySpaces(QList<int> &list);

    /* последовательно выполняет геномы всех кланов, добавленных в мир
     * при несоответсвии размеров img и размеров мира возвращает false
     * требуется img такого формата, где клану соответсвут один пиксель
     * при перемещении клана, ставит его цвет на новое место, а в старое устанавливает QColor(0,0,0,0)
     */
    bool run(QImage &img);
private:
    QList<Clan*> _clans;
    int _w, _h;
};


#endif // WORLD_H
