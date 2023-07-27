#include <QMouseEvent>
#include <QApplication>
#include <QScrollBar>
#include "graphicsviewzoom.h"

GraphicsViewZoom::GraphicsViewZoom(QGraphicsView* view)
    : QObject(view), _view(view)
{
    _view->viewport()->installEventFilter(this);
    _view->setMouseTracking(true);
    _modifiers = Qt::ControlModifier;
    _zoomFactorBase = 1.0015;
}

GraphicsViewZoom::~GraphicsViewZoom()
{
}

void GraphicsViewZoom::setModifiers(Qt::KeyboardModifiers modifiers)
{
    _modifiers = modifiers;
}

void GraphicsViewZoom::setZoomFactorBase(double value) {
    _zoomFactorBase = value;
}

void GraphicsViewZoom::zooming(double factor)
{
    _view->scale(factor, factor);
    _view->centerOn(_targetScenePos);
    QPointF deltaViewportPos = _targetViewportPos - QPointF(_view->viewport()->width() / 2.0,
                                                           _view->viewport()->height() / 2.0);
    QPointF viewportCenter = _view->mapFromScene(_targetScenePos) - deltaViewportPos;
    _view->centerOn(_view->mapToScene(viewportCenter.toPoint()));
}

bool GraphicsViewZoom::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPointF delta = _targetViewportPos - mouseEvent->pos();
        if (qAbs(delta.x()) > 5 || qAbs(delta.y()) > 5)
        {
          _targetViewportPos = mouseEvent->pos();
          _targetScenePos = _view->mapToScene(mouseEvent->pos());
        }
    }
    else if (event->type() == QEvent::Wheel)
    {
        QWheelEvent* wheelEvent  = static_cast<QWheelEvent*>(event);
        if (QApplication::keyboardModifiers() == _modifiers)
        {
            double angle = wheelEvent ->angleDelta().y();
            double factor = qPow(_zoomFactorBase, angle);
            zooming(factor);
            return true;
        }
    }
    return false;
}
