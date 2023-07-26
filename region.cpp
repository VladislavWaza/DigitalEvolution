#include "region.h"

Region::Region(QGraphicsItem *parent)
    :QGraphicsRectItem(parent)
{
}

Region::Region(const QRectF &rect, QGraphicsItem *parent)
   :QGraphicsRectItem(rect, parent)
{
}

Region::Region(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent)
    :QGraphicsRectItem(x, y, width, height, parent)
{
}

Region::~Region()
{
}
