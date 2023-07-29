#ifndef CLAN_H
#define CLAN_H

#include <QGraphicsRectItem>

class Clan : public QGraphicsRectItem
{
public:
    enum class GenomeInitType{Random};
    Clan(GenomeInitType genomeInitType = GenomeInitType::Random, QGraphicsItem *parent = nullptr);
    ~Clan();

    getGenom(uint8_t *ptr) const;

    static const int _size = 8; //должно делиться на 4
    static const QPoint _directions[];
private:
    uint8_t* _genom;
};

#endif // CLAN_H
