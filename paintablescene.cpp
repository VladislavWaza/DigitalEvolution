#include "paintablescene.h"

PaintableScene::PaintableScene(QObject *parent)
    : QGraphicsScene{parent}
{
    _isPressed = false;
}

void PaintableScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::RightButton)
    {
        _isPressed = true;
        emit signalPainting(mouseEvent);
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void PaintableScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (_isPressed)
        emit signalPainting(mouseEvent);
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void PaintableScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::RightButton)
        _isPressed = false;
    QGraphicsScene::mousePressEvent(mouseEvent);
}
