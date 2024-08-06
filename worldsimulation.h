#ifndef WORLDSIMULATION_H
#define WORLDSIMULATION_H

#include <QImage>
#include <QRandomGenerator>

#include <vector>
#include <list>

struct Region
{
};

struct Cell
{
    size_t x = 0;
    size_t y = 0;
    Cell(size_t x, size_t y) : x(x), y(y) {}
};

class WorldSimulation
{
public:
    enum class CellsDetailLevel : size_t {OnePixel = 1, Square3x3 = 3};

    WorldSimulation(size_t worldWidth, size_t worldHidth, CellsDetailLevel detailLevel = CellsDetailLevel::OnePixel);

    size_t worldWidth() const {return m_width;}
    size_t worldHeight() const {return m_height;}

    size_t imageWidth() const {return m_width * static_cast<size_t>(m_detailLevel);}
    size_t imageHeight() const {return m_height * static_cast<size_t>(m_detailLevel);}
    size_t cellSize() const {return static_cast<size_t>(m_detailLevel);}

    size_t cellsCount() const {return m_cellOrder.size();}

    QImage getImage();
    void run();
    void addCells(size_t count);

private:
    static std::vector<QPoint> sample(std::vector<QPoint> &seq, size_t count);


    size_t m_width = 0;
    size_t m_height = 0;

    CellsDetailLevel m_detailLevel;
    std::vector<Region> m_regions;
    std::list<Cell> m_cellOrder;
    std::vector<Cell*> m_cells;

    QImage m_image;
};

#endif // WORLDSIMULATION_H
