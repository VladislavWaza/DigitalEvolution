#include "worldsimulation.h"

#include <random>

namespace DigitalEvolution
{

WorldSimulation::WorldSimulation(int worldWidth, int worldHidth, CellsDetailLevel detailLevel)
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
    const double maxEnergyCoef = 1.0 / log(m_maxEnergy);
    QRgb* data = reinterpret_cast<QRgb*>(m_image.bits());
    const size_t imgWidth = m_image.width();
    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < m_width; ++x)
        {
            const size_t cellIndex = y * m_width + x;
            QRgb color = 0xffffffff;
            if (m_cells[cellIndex])
            {
                if (m_displayMode == DisplayMode::CellType)
                    color = m_cells[cellIndex]->color();
                else if (m_displayMode == DisplayMode::Energy)
                {
                    int blue = log(m_cells[cellIndex]->allEnergy()) * maxEnergyCoef * 256;
                    if (blue > 255)
                        color = 0xff00ffff;
                    else if (blue > 200)
                        color = qRgb(0, blue, blue);
                    else
                        color = qRgb(0, 0, blue);
                }
            }

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
        }
    }
    return m_image;
}

void WorldSimulation::run()
{
    m_maxEnergy = 0;
    ++m_stepsNumber;
    for (m_curCell = m_cellOrder.begin(); m_curCell != m_cellOrder.end();)
    {
        auto& cell = *m_curCell;
        cell->doAct(*this);
        if (cell->isDead())
        {
            //удаление из m_cells происходит через preEraseCell внутри Cell

            //сейчас этот код бесполезен
            //if (m_cells[cell->y() * m_width + cell->x()] == cell.get())
            //    m_cells[cell->y() * m_width + cell->x()] = nullptr;

            m_curCell = m_cellOrder.erase(m_curCell);
        }
        else
        {
            ++m_curCell;
            m_maxEnergy = m_maxEnergy >= cell->allEnergy() ? m_maxEnergy : cell->allEnergy();
        }
    }
}

void WorldSimulation::addCells(size_t count)
{
    std::vector<QPoint> emptySpaces;
    for (int y = 0; y < m_height; ++y)
        for (int x = 0; x < m_width; ++x)
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
        m_cellOrder.push_back(std::make_unique<Sprout>(x, y, DigitalEvolution::ENERGY_START));
        m_cells[y * m_width + x] = m_cellOrder.back().get();
    }
}

QPoint WorldSimulation::getLeftPos(int x, int y) const
{
    if (x > m_width || y > m_height)
        throw std::out_of_range("incorrect x and y coordinates");
    x -= 1;
    return returnPosToWorld(x, y);
}

QPoint WorldSimulation::getRightPos(int x, int y) const
{
    if (x > m_width || y > m_height)
        throw std::out_of_range("incorrect x and y coordinates");
    x += 1;
    return returnPosToWorld(x, y);
}

QPoint WorldSimulation::getUpPos(int x, int y) const
{
    if (x > m_width || y > m_height)
        throw std::out_of_range("incorrect x and y coordinates");
    y -= 1;
    return returnPosToWorld(x, y);
}

QPoint WorldSimulation::getDownPos(int x, int y) const
{
    if (x > m_width || y > m_height)
        throw std::out_of_range("incorrect x and y coordinates");
    y += 1;
    return returnPosToWorld(x, y);
}

QPoint WorldSimulation::getNeighborPos(int x, int y, Direction dir) const
{
    if (dir == Direction::Left)
        return getLeftPos(x, y);
    else if (dir == Direction::Up)
        return getUpPos(x, y);
    else if (dir == Direction::Right)
        return getRightPos(x, y);
    else if (dir == Direction::Down)
        return getDownPos(x, y);
    else
        throw std::runtime_error("None direction in WorldSimulation::getPos");

    return QPoint();
}

QPoint WorldSimulation::returnPosToWorld(int x, int y) const
{
    x %= m_width;
    if (x < 0)
        x += m_width;
    y %= m_height;
    if (y < 0)
        y += m_height;
    return {x, y};
}

Cell *WorldSimulation::insertCellBeforeCur(std::unique_ptr<Cell> &&cell)
{
    if (cell && m_cells[cell->y() * m_width + cell->x()] == nullptr)
    {
        auto& newCell = *m_cellOrder.insert(m_curCell, std::move(cell));
        if (newCell)
            m_cells[newCell->y() * m_width + newCell->x()] = newCell.get();
        return newCell.get();
    }
    return nullptr;
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

}
