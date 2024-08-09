#ifndef WORLDSIMULATION_H
#define WORLDSIMULATION_H

#include "cell.h"

#include <QImage>

#include <vector>
#include <list>

namespace DigitalEvolution
{

const int ENERGY_START = 200;

struct Region
{
};

class WorldSimulation
{
public:
    enum class CellsDetailLevel : size_t {OnePixel = 1, Square3x3 = 3, Square5x5 = 5};

    WorldSimulation(int worldWidth, int worldHidth, CellsDetailLevel detailLevel = CellsDetailLevel::OnePixel);

    size_t worldWidth() const {return m_width;}
    size_t worldHeight() const {return m_height;}
    size_t stepsNumber() const {return m_stepsNumber;}

    size_t imageWidth() const {return m_width * static_cast<size_t>(m_detailLevel);}
    size_t imageHeight() const {return m_height * static_cast<size_t>(m_detailLevel);}
    size_t cellSize() const {return static_cast<size_t>(m_detailLevel);}

    size_t cellsCount() const {return m_cellOrder.size();}

    QImage getImage();
    void run();
    void addCells(size_t count);

    QPoint getLeftPos(int x, int y) const;
    QPoint getRightPos(int x, int y) const;
    QPoint getUpPos(int x, int y) const;
    QPoint getDownPos(int x, int y) const;
    QPoint returnPosToWorld(int x, int y) const;

    Cell *getCell(int x, int y) { return m_cells[y * m_width + x]; }
    Cell *getCell(const QPoint& point) { return m_cells[point.y() * m_width + point.x()]; }
    void preEraseCell(int x, int y) { m_cells[y * m_width + x] = nullptr; }
    Cell *insertCellBeforeCur(std::unique_ptr<Cell>&& cell);

private:
    static std::vector<QPoint> sample(std::vector<QPoint> &seq, size_t count);

    int m_width = 0;
    int m_height = 0;
    size_t m_stepsNumber = 0;

    CellsDetailLevel m_detailLevel;
    std::vector<Region> m_regions;
    std::list<std::unique_ptr<Cell>> m_cellOrder;
    std::list<std::unique_ptr<Cell>>::iterator m_curCell;
    std::vector<Cell*> m_cells;

    QImage m_image;
};


}

#endif // WORLDSIMULATION_H
