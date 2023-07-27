#ifndef GRAPHICSVIEWZOOM_H
#define GRAPHICSVIEWZOOM_H

#include <QObject>
#include <QGraphicsView>

/* Этот класс добавляет возможность масштабирования QGraphicsView с помощью колесика мыши.
 * Точка под курсором остается неподвижной, пока это возможно.
 *
 * Использование:
 *    new GraphicsViewZoom(view);
 *
 * Объект будет удален автоматически при удалении view.
 *
 * Вы можете установить модификаторы клавиатуры, используемые для масштабирования, с помощью setModifiers().
 * Масштабирование будет выполнено только при точном совпадении комбинации модификаторов.
 * Модификатором по умолчанию является Ctrl.
 *
 * Вы можете изменить скорость масштабирования, вызвав функцию setZoomFactorBase().
 * Коэффициент масштабирования рассчитывается как zoomFactorBase^angleDelta
 * (см. QWheelEvent::angleDelta).
 * Базовый коэффициент масштабирования по умолчанию равен 1.0015.
 */

class GraphicsViewZoom : public QObject
{
    Q_OBJECT
public:
    GraphicsViewZoom(QGraphicsView* view);
    ~GraphicsViewZoom();
    void setModifiers(Qt::KeyboardModifiers modifiers);
    void setZoomFactorBase(double value);

private:
    QGraphicsView* _view;
    Qt::KeyboardModifiers _modifiers;
    double _zoomFactorBase;
    QPointF _targetScenePos, _targetViewportPos;

    void zooming(double factor);
    bool eventFilter(QObject* object, QEvent* event);
};

#endif // GRAPHICSVIEWZOOM_H
