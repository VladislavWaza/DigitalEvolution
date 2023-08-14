#ifndef CELL_H
#define CELL_H
#include <QPoint>
#include <QObject>
#include <QColor>
class Cell : public QObject
{
    Q_OBJECT
public:
    enum class GenomeInitType{Random, Collector};
    Cell(GenomeInitType genomeInitType = GenomeInitType::Random);
    Cell(const Cell& parent);
    ~Cell();

    QColor getColor();

    void getGenom(uint8_t *ptr) const;

    //возвращает число отличающихся генов в геноме
    int calculateGenomesDiff(const Cell& other) const;

    void setDirection(QPoint direction);
    QPoint getDirection();

    int getStrength();
    void setStrength(int strength);

    int getFood();
    void increaseFood(int food);

    void increaseAge();
    int getAge();

    bool isAlive();
    void kill();

    /*вычитает еду за переход на следующий шаг, увеличивет возраст
     *при недостаточном количестве еды или при старости убивает клетку
     */
    void survive();

    static const int _maxFood = 1000;
    static const int _maxAge = 2000;
    static const int _percentMutation = 50;
    static const int _maxGene = 23;
    static const int _maxStrength = 4;
    static const int _size = 12; //должно делиться на 4
    static const QPoint _directions[];
signals:
    void signalKilled();
private:
    uint8_t* _genom;
    int _food;
    int _strength;
    int _age;
    QPoint _direction;
    bool _isAlive;
    QColor _color;
};

#endif // CELL_H
