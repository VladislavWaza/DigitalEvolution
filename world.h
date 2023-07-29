#ifndef WORLD_H
#define WORLD_H

#include "clan.h"
#include "region.h"

class World
{
public:
    World(int squareSide, int w = 0, int h = 0);
    void addItem(int x, int y, Clan *clan);
    void run();
private:
    QList<Clan*> _clans;
    int _w, _h, _squareSide;
};


#endif // WORLD_H
