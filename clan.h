#ifndef CLAN_H
#define CLAN_H
#include <QPoint>

class Clan
{
public:
    enum class GenomeInitType{Random};
    Clan(GenomeInitType genomeInitType = GenomeInitType::Random);
    ~Clan();

    void getGenom(uint8_t *ptr) const;

    static const int _size = 8; //должно делиться на 4
    static const QPoint _directions[];
private:
    uint8_t* _genom;
};

#endif // CLAN_H
