#include "worldsimulation.h"

WorldSimulation::WorldSimulation(size_t worldWidth, size_t worldHidth, CellsDetailLevel detailLevel)
    : m_width(worldWidth),
      m_height(worldHidth),
      m_detailLevel(detailLevel),
      m_regions(worldWidth * worldHidth)
{
    m_image = QImage(m_width * static_cast<size_t>(detailLevel),
                     m_height * static_cast<size_t>(detailLevel),
                     QImage::Format_ARGB32_Premultiplied);
}

QImage WorldSimulation::getImage()
{
    return m_image;
}
