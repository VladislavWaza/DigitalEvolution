#include <QAbstractGraphicsShapeItem>
#include "paintablescene.h"

PaintableScene::PaintableScene(QObject *parent)
    : QGraphicsScene{parent}
{
}

void PaintableScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    emit signalPainting(mouseEvent);
    QGraphicsScene::mousePressEvent(mouseEvent);
}
