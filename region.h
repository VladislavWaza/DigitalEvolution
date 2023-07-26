#ifndef REGION_H
#define REGION_H

#include <QGraphicsRectItem>

class Region : public QGraphicsRectItem
{
public:
    Region(QGraphicsItem *parent = nullptr);
    Region(const QRectF &rect, QGraphicsItem *parent = nullptr);
    Region(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent = nullptr);
    ~Region();
};

#endif // REGION_H
