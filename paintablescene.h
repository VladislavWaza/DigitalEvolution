#ifndef PAINTABLESCENE_H
#define PAINTABLESCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

/* Класс нужен для отслеживания нажатия и перемещения мыши
 * и отправления сигнала в главное окно для последующей обработки
*/

class PaintableScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit PaintableScene(QObject *parent = nullptr);
signals:
    void signalPainting(QGraphicsSceneMouseEvent *mouseEvent);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
private:
    bool _isPressed;
};

#endif // PAINTABLESCENE_H
