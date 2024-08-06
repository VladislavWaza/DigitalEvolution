#ifndef WORLDSIMULATION_H
#define WORLDSIMULATION_H

#include <QImage>

#include <vector>

struct Region
{
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

    QImage getImage();

private:
    size_t m_width = 0;
    size_t m_height = 0;

    CellsDetailLevel m_detailLevel;
    std::vector<Region> m_regions;

    QImage m_image;
};

#endif // WORLDSIMULATION_H
