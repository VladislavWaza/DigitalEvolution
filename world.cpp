#include <QRandomGenerator>
#include "world.h"

QPoint const World::CellUndefined = QPoint(-1,-1);


World::World(int w, int h)
    :_cells(w*h), _regions(w*h), _w(w), _h(h)
{
}

World::~World()
{
    for (int i = 0; i < _w * _h; ++i)
    {
        delete _cells[i];
        delete _regions[i];
    }

}

int World::width()
{
    return _w;
}

int World::height()
{
    return _h;
}

void World::getCellsImage(QImage &img, DisplayMode mode)
{
    img = img.scaled(_w, _h);
    img = img.convertedTo(QImage::Format_ARGB32);
    Cell *cell;
    QRgb* data = reinterpret_cast<QRgb*>(img.bits());
    for (int x = 0; x < _w; ++x)
    {
        for (int y = 0; y < _h; ++y)
        {
            qsizetype index = x * _h + y;
            cell = _cells[index];
            if (cell)
            {
                if (mode == DisplayMode::Сommon)
                {
                    data[index] = cell->getColor().rgba();
                }
                else if (mode == DisplayMode::Food)
                {
                    float green = cell->getFood();
                    if (green < 0)
                        green = 0;
                    green /= Cell::_maxFood;
                    data[index] = QColor::fromRgbF(1 - green, green, 0, 1).rgba();
                }
                else if (mode == DisplayMode::Strength)
                {
                    data[index] = QColor(255 / static_cast<float>(Cell::_maxStrength) * cell->getStrength(),0,0,255).rgba();
                }
                else if (mode == DisplayMode::Age)
                {
                    int blue = 255 / static_cast<float>(Cell::_maxAge / 10) * cell->getAge();
                    if (blue > 255)
                        blue = 255;
                    int red = 255 / static_cast<float>(Cell::_maxAge) * cell->getAge();
                    data[index] = QColor(red, 0, blue,255).rgba();
                }

            }
            else
                data[index] = qRgba(0,0,0,0);
        }
    }
}

void World::getRegionsImage(QImage &img, DisplayMode mode)
{
    img = img.scaled(_w, _h);
    img = img.convertedTo(QImage::Format_ARGB32);
    Region *region;
    QRgb* data = reinterpret_cast<QRgb*>(img.bits());
    const float minLight = 1.f / Region::_maxLight;
    for (int x = 0; x < _w; ++x)
    {
        for (int y = 0; y < _h; ++y)
        {
            qsizetype index = x * _h + y;
            region = _regions[index];
            if (region)
            {
                float bright = region->getLight() * minLight;
                data[index] = QColor::fromRgbF(bright, bright, bright).rgba();
            }
            else
                data[index] = qRgba(0,0,0,0);
        }
    }
}

Cell *World::getCell(int x, int y)
{
    if (x < 0 || x >= _w || y < 0 || y >= _h)
        return nullptr;
    return _cells[x * _h + y];
}

QPoint World::getCellPos(Cell *cell)
{
    if (!cell)
        return QPoint(-1,-1);
    for (int x = 0; x < _w; ++x)
    {
        for (int y = 0; y < _h; ++y)
        {
            if (_cells[x * _h + y] == cell)
                return QPoint(x,y);
        }
    }
    return QPoint(-1,-1);
}

Region *World::getRegion(int x, int y)
{
    if (x < 0 || x >= _w || y < 0 || y >= _h)
        return nullptr;
    return _regions[x * _h + y];
}

bool World::addCell(int x, int y, Cell *cell)
{
    if (x < 0 || x >= _w || y < 0 || y >= _h)
        return false;
    if (_cells[x * _h + y])
        return false;
    _cells[x * _h + y] = cell;
    return true;
}

bool World::setRegion(int x, int y, Region *region)
{
    if (x < 0 || x >= _w || y < 0 || y >= _h)
        return false;
    if (_regions[x * _h + y])
        delete _regions[x * _h + y];
    _regions[x * _h + y] = region;
    return true;
}

void World::getNumsOfEmptySpaces(QList<int> &list)
{
    list.clear();
    for (int i = 0; i < _w * _h; ++i)
    {
        if (!_cells[i])
            list.append(i);
    }
}

QPoint World::randomEmptySpaceNearby(QPoint pos, int distance)
{
    QVector<QPoint> seq;
    QPoint tempPos, newPos;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 1; j <= distance; ++j)
        {
            tempPos = pos + j * Cell::_directions[i];
            returnPosToWorld(&tempPos);
            if (_cells[tempPos.x() * _h + tempPos.y()]) //если встретили препятствие
            {
                if (j != 1) //если пустое место уже встречалось
                {
                    newPos = tempPos - Cell::_directions[i]; //возвращаемся назад
                    returnPosToWorld(&newPos);
                    seq.append(newPos); //добавляем в последовательность
                }
                break;
            }
            else if (j == distance) //дошли до желаемого расстояния и не встретили препятствия
            {
                seq.append(tempPos); //добавляем в последовательность
            }
        }
    }
    if (seq.isEmpty())
        return World::CellUndefined;
    return seq[QRandomGenerator::system()->bounded(seq.size())];
}

int World::cellsNumber()
{
    int n = 0;
    for (int i = 0; i < _w * _h; ++i)
    {
        if (_cells[i])
            ++n;
    }
    return n;
}

void World::run()
{
    QList<Cell*> oldCells(_cells);
    Cell *cell;
    uint8_t genom[Cell::_size];

    for (int x = 0; x < _w; ++x)
    {
        for (int y = 0; y < _h; ++y)
        {
            cell = oldCells[x * _h + y];
            if (cell && cell->isAlive())
            {
                QPoint pos;
                int operationCounter = 0;
                int semiFinalOperationCounter = 0;
                pos.rx() = x;
                pos.ry() = y;
                cell->getGenom(genom);

                //выполняем команды генома
                for (int i = 0; i < Cell::_size; ++i)
                {
                    //если было совершено много операций выходим
                    if (operationCounter >= 15)
                        break;
                    ++operationCounter;

                    //если было совершено 2 завершающие операции выходим
                    if (semiFinalOperationCounter >= 2)
                        break;


                    if (genom[i] >= 0 && genom[i] <= 7) //поворот
                    {
                        cell->setDirection(Cell::_directions[genom[i]]);
                    }
                    if (genom[i] == 8 || genom[i] == 9) //движение
                    {
                        if (move(&pos, cell))
                            ++semiFinalOperationCounter;
                    }
                    if (genom[i] == 10 || genom[i] == 11) //сбор еды
                    {
                        collectFood(pos, cell);
                        ++semiFinalOperationCounter;
                    }
                    if (genom[i] == 12 || genom[i] == 13) //атака
                    {
                        if (attack(&pos, cell))
                            ++semiFinalOperationCounter;
                    }
                    if (genom[i] >= 14 && genom[i] <= 17) //изменение силы
                    {
                        aimStrength(genom[i] - 13, cell);
                    }
                    if (genom[i] == 18 || genom[i] == 19) // условные операции
                    {
                        if ((genom[i] == 18 && isEmptyAhead(pos, cell)) || //пусто ли впереди
                            (genom[i] == 19 && isRelativeAhead(pos, cell))) //родственник ли впереди
                        {
                            //перешагиваем через один ген и выполняем геном дальше
                            ++i;
                            continue;
                        }
                        else
                        {
                            //переходим в ген указанный в следующем гене и выполняем геном оттуда
                            if (i + 1 < Cell::_size)
                            {
                                i = genom[i + 1] - 1;
                                continue;
                            }
                        }
                    }
                    if (genom[i] == 20 || genom[i] == 21 || genom[i] == 22)
                    {
                        born(pos, cell, genom[i] - 19);
                    }
                    if (genom[i] == 23)
                    {
                        born(pos, cell, 10);
                    }
                }

                //отпочковываем потомка
                if (cell->getFood() >= 300)
                    born(pos, cell, 1);

                //приверяем выживает ли клетка, если нет убираем её
                cell->survive();
                if (!cell->isAlive())
                {
                    _cells[pos.x() * _h + pos.y()] = nullptr;
                }
            }
        }
    }

    //удаляем мертвые клетки
    for (int x = 0; x < _w; ++x)
    {
        for (int y = 0; y < _h; ++y)
        {
            cell = oldCells[x * _h + y];
            if (cell && !cell->isAlive())
            {
                delete cell;
            }
        }
    }
}

bool World::move(QPoint *pos, Cell *cell)
{
    cell->increaseFood(-2);
    //вычисляем место перемещения
    QPoint newPoint(*pos);
    newPoint += cell->getDirection();
    returnPosToWorld(&newPoint);

    if (!_cells[newPoint.x() * _h + newPoint.y()]) //если не занято
    {
        _cells[newPoint.x() * _h + newPoint.y()] = cell; //перемещаемся
        _cells[pos->x() * _h + pos->y()] = nullptr; //прошлое место опустошаем
        *pos = newPoint;
        return true;
    }
    return false;
}

void World::collectFood(QPoint pos, Cell *cell)
{
    cell->increaseFood(_regions[pos.x() * _h + pos.y()]->getLight());
}

bool World::attack(QPoint *pos, Cell *cell)
{
    cell->increaseFood(-2);
    //вычисляем место атаки
    QPoint target(*pos);
    target += cell->getDirection();
    returnPosToWorld(&target);

    Cell *enemy = _cells[target.x() * _h + target.y()];
    if (enemy && enemy->getStrength() <= cell->getStrength())
    {
        _cells[target.x() * _h + target.y()] = nullptr;
        cell->increaseFood(enemy->getFood() * 2 / 3);
        enemy->kill();
        return true;
    }
    return false;
}

bool World::born(QPoint pos, Cell *cell, int dist)
{
    if (cell->getFood() >= 200)
    {
        QPoint newPos = randomEmptySpaceNearby(pos, dist);
        if (newPos == World::CellUndefined)
            return false;
        Cell* son = new Cell(*cell);
        _cells[newPos.x() * _h + newPos.y()] = son;
        cell->increaseFood(-200);
        son->increaseFood(10);
        return true;
    }
    return false;
}

void World::aimStrength(int target, Cell *cell)
{
    if (cell->getStrength() != target && cell->getFood() >= 50)
    {
        if (cell->getStrength() - target > 0)
            cell->setStrength(cell->getStrength() - 1);
        else
            cell->setStrength(cell->getStrength() + 1);
        cell->increaseFood(-50);
    }
}

void World::returnPosToWorld(QPoint *pos)
{
    pos->rx() %= _w;
    if (pos->x() < 0)
        pos->rx() += _w;
    pos->ry() %= _h;
    if (pos->y() < 0)
        pos->ry() += _h;
}

bool World::isEmptyAhead(QPoint pos, Cell *cell)
{
    QPoint newPoint(pos);
    newPoint += cell->getDirection();
    returnPosToWorld(&newPoint);
    if (!_cells[newPoint.x() * _h + newPoint.y()]) //если не занято
        return true;
    return false;
}

bool World::isRelativeAhead(QPoint pos, Cell *cell)
{
    QPoint newPoint(pos);
    newPoint += cell->getDirection();
    returnPosToWorld(&newPoint);
    if (_cells[newPoint.x() * _h + newPoint.y()]) //если не пусто
    {
        Cell* cell2 = _cells[newPoint.x() * _h + newPoint.y()];
        if (cell->calculateGenomesDiff(*cell2) <= 2)
            return true;
    }
    return false;
}
