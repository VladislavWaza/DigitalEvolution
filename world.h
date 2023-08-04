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
    static const QPoint ClanUndefined;
    enum class DisplayMode{Сommon, Food, Population};

    World(int w = 0, int h = 0);
    ~World();

    int width();
    int height();

    void getClansImage(QImage &img, DisplayMode mode);
    void getRegionsImage(QImage &img, DisplayMode mode);

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

    QPoint randomEmptySpaceNearby(QPoint pos);

    /* возвращает число живых кланов*/
    int clansNumber();

    /* последовательно выполняет геномы всех кланов, добавленных в мир
     * при несоответсвии размеров img и размеров мира возвращает false
     * требуется img такого формата, где клану соответсвут один пиксель
     *
     * Алгоритм работы мира:
     * Для каждого клана по очереди
     * 1.Проиходим по геному и выполняем команды
     * 2.Есть команды повернуть, атаковать, собрать еду, перейти
     * 3.Команды собрать еду, атаковать и перейти - завершающие
     * 4.Но если не было кого атаковать или некуда перейти, то не завершающие
     * 5.Проверка выживает ли клан
     */
    void run();

    //если это возможно, то передвинуть клан вперед согласно его направлению, иначе false
    bool move(QPoint *pos, Clan *clan);

    void collectFood(QPoint pos, Clan *clan);

    //если это возможно, то убить клан по направлению поворота и забрать его еду, иначе false
    bool attack(QPoint *pos, Clan *clan);

    bool born(QPoint pos, Clan *clan);

    void returnPosToWorld(QPoint *pos);
private:
    QList<Clan*> _clans;
    QList<Region*> _regions;
    int _w, _h;
};


#endif // WORLD_H
