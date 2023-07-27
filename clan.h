#ifndef CLAN_H
#define CLAN_H

#include <QGraphicsRectItem>

class Clan : public QGraphicsRectItem
{
public:
    enum class GenomeInitType{Random};
    Clan(GenomeInitType genomeInitType = GenomeInitType::Random, QGraphicsItem *parent = nullptr);
    ~Clan();

    void run();

private:
    uint8_t* _genom;
    static const int _size = 8; //должно делиться на 4
};

#endif // CLAN_H
