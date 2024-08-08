#include "worldsimulation.h"

#include <random>

WorldSimulation::WorldSimulation(size_t worldWidth, size_t worldHidth, CellsDetailLevel detailLevel)
    : m_width(worldWidth),
      m_height(worldHidth),
      m_detailLevel(detailLevel),
      m_regions(worldWidth * worldHidth),
      m_cells(worldWidth * worldHidth)
{
    m_image = QImage(m_width * static_cast<size_t>(detailLevel),
                     m_height * static_cast<size_t>(detailLevel),
                     QImage::Format_ARGB32_Premultiplied);
    m_image.fill(Qt::white);

    for (size_t i = 0; i < m_cells.size(); ++i)
        m_cells[i] = nullptr;
}

QImage WorldSimulation::getImage()
{
    QRgb* data = reinterpret_cast<QRgb*>(m_image.bits());
    const size_t imgWidth = m_image.width();
    for (size_t y = 0; y < m_height; ++y)
    {
        for (size_t x = 0; x < m_width; ++x)
        {
            const size_t cellIndex = y * m_width + x;
            QRgb color = m_cells[cellIndex] ? m_cells[cellIndex]->color() : 0xffffffff;

            if (m_detailLevel == CellsDetailLevel::OnePixel)
                data[y * m_image.width() + x] = color;
            else if (m_detailLevel == CellsDetailLevel::Square3x3)
            {
                const size_t factor = static_cast<size_t>(CellsDetailLevel::Square3x3);
                data[(y * factor + 0) * imgWidth + (x * factor + 1)] = color;
                data[(y * factor + 1) * imgWidth + (x * factor + 0)] = color;
                data[(y * factor + 1) * imgWidth + (x * factor + 1)] = color;
                data[(y * factor + 1) * imgWidth + (x * factor + 2)] = color;
                data[(y * factor + 2) * imgWidth + (x * factor + 1)] = color;
            }
            else if (m_detailLevel == CellsDetailLevel::Square5x5)
            {
                const size_t factor = static_cast<size_t>(CellsDetailLevel::Square5x5);
                data[(y * factor + 0) * imgWidth + (x * factor + 2)] = color;
                data[(y * factor + 1) * imgWidth + (x * factor + 1)] = color;
                data[(y * factor + 1) * imgWidth + (x * factor + 2)] = color;
                data[(y * factor + 1) * imgWidth + (x * factor + 3)] = color;
                data[(y * factor + 2) * imgWidth + (x * factor + 0)] = color;
                data[(y * factor + 2) * imgWidth + (x * factor + 1)] = color;
                data[(y * factor + 2) * imgWidth + (x * factor + 2)] = color;
                data[(y * factor + 2) * imgWidth + (x * factor + 3)] = color;
                data[(y * factor + 2) * imgWidth + (x * factor + 4)] = color;
                data[(y * factor + 3) * imgWidth + (x * factor + 1)] = color;
                data[(y * factor + 3) * imgWidth + (x * factor + 2)] = color;
                data[(y * factor + 3) * imgWidth + (x * factor + 3)] = color;
                data[(y * factor + 4) * imgWidth + (x * factor + 2)] = color;
            }
        }
    }
    return m_image;
}

void WorldSimulation::run()
{
    ++m_stepsNumber;
    for (auto& cell: m_cellOrder)
    {
        cell->doAct(*this);
    }
}

void WorldSimulation::addCells(size_t count)
{
    std::vector<QPoint> emptySpaces;
    for (size_t y = 0; y < m_height; ++y)
        for (size_t x = 0; x < m_width; ++x)
            if (!m_cells[y * m_width + x])
                emptySpaces.push_back(QPoint(x, y));

    if (emptySpaces.size() < count)
        count = emptySpaces.size();
    else
        emptySpaces = sample(emptySpaces, count);

    for (size_t i = 0; i < count; ++i)
    {
        int x = emptySpaces[i].x();
        int y = emptySpaces[i].y();
        m_cellOrder.push_back(std::make_unique<Leaf>(x, y));
        m_cells[y * m_width + x] = m_cellOrder.back().get();
    }
}

std::vector<QPoint> WorldSimulation::sample(std::vector<QPoint> &seq, size_t count)
{
    if (count > seq.size())
        return seq;

    std::vector<QPoint> result;
    result.reserve(count);

    std::shuffle(seq.begin(), seq.end(), std::mt19937(std::random_device{}()));
    result.insert(result.end(), seq.begin(), seq.begin() + count);

    return result;
}
