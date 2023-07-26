#ifndef PAINTABLESCENE_H
#define PAINTABLESCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

class PaintableScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit PaintableScene(QObject *parent = nullptr);
signals:
    void signalPainting(QGraphicsSceneMouseEvent *mouseEvent);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
};

#endif // PAINTABLESCENE_H
