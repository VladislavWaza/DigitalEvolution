#ifndef CLAN_H
#define CLAN_H
#include <QPoint>
#include <QObject>
class Clan : public QObject
{
    Q_OBJECT
public:
    enum class GenomeInitType{Random};
    Clan(GenomeInitType genomeInitType = GenomeInitType::Random);
    ~Clan();

    void getGenom(uint8_t *ptr) const;

    void setDirection(QPoint direction);
    QPoint getDirection();

    int getPopulation();

    int getFood();
    void increaseFood(int food);

    bool isAlive();
    void kill();

    void survive();

    static const int _size = 8; //должно делиться на 4
    static const QPoint _directions[];
signals:
    void signalKilled();
private:
    uint8_t* _genom;
    int _food, _population;
    QPoint _direction;
    bool _isAlive;
};

#endif // CLAN_H
