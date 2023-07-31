#ifndef WORLD_H
#define WORLD_H

#include <QImage>
#include "clan.h"
#include "region.h"

/* Нужен чтобы забрать задачу работы с миром у MainWinwow
 *
 * Отслеживает положения кланов(class Clan) в мире, храня указатели на кланы
 * Выполняет их геномы
 * Автоматически удаляет кланы и регионы при удалении мира
 */


class World
{
public:
    World(int w = 0, int h = 0);
    ~World();

    int width();
    int height();

    /*Возвращает указатель на клан по указанным координатам
    * при координатах выходящих за границы мира или при отсутсвии клана возвращает nullptr
    */
    Clan *getClan(int x, int y);

    /*Возвращает координаты клана в мире, при отсутвии клана возвращает World::ClanUndefined = QPoint(-1,-1)*/
    QPoint getClanPos(Clan *clan);

    /*Возвращает указатель на регион по указанным координатам
    * при координатах выходящих за границы мира возвращает nullptr
    */
    Region *getRegion(int x, int y);


    /* Добавляет клан в мир и возвращает true
     * при попытке добавления за пределы мира возращает false
     * при попытке добавления в уже занятый регион возвращает false
     */
    bool addClan(int x, int y, Clan *clan);

    /* Добавляет регион в мир и возвращает true
     * при попытке добавления в уже занятый регион заменяет, а старый удаляет
     * при попытке добавления за пределы мира возращает false
     */
    bool setRegion(int x, int y, Region *region);

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

    static const QPoint ClanUndefined;
private:
    QList<Clan*> _clans;
    QList<Region*> _regions;
    int _w, _h;
};


#endif // WORLD_H
