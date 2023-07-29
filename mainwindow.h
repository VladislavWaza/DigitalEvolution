#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "paintablescene.h"
#include "world.h"
#include "region.h"
#include "clan.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum class LayerLevel{Region = 0, Clan = 1, Grid = 2};
private slots:
    void on_createWorld_clicked();
    void on_addClans_clicked();
    void on_start_clicked();

    void run();

    void slotPainting(QGraphicsSceneMouseEvent *mouseEvent);
    void slotMidButton(QGraphicsSceneMouseEvent *mouseEvent);

    QVector<int> sample(QVector<int>& seq, int count);
private:
    void addGrid();


    Ui::MainWindow *ui;
    PaintableScene *_scene;
    QGraphicsPixmapItem *_clansItem, *_regionsItem;
    QGraphicsPathItem *_grid;
    World *_world;
    int _clansNumber;
    QTimer _timer;
    int _ms;
};
#endif // MAINWINDOW_H
