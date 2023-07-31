#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
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

    enum class LayerLevel{Region = 0, Clan = 1, Grid = 2, HighlightSelection = 3};
private slots:
    void on_createWorld_clicked();
    void on_addClans_clicked();
    void on_start_clicked();

    void run();

    void slotSelectedClanKilled();
    void slotPainting(QGraphicsSceneMouseEvent *mouseEvent);
    void slotMidButton(QGraphicsSceneMouseEvent *mouseEvent);

    QVector<int> sample(QVector<int>& seq, int count);
private:
    void addGrid();

    // отображает информацию о выбранных территории и клане
    void displayInfo();

    // отображает рамку по указанным координтам
    void displaySelection(int x, int y);
    Ui::MainWindow *ui;
    PaintableScene *_scene;
    World *_world;

    QGraphicsPixmapItem *_clansItem, *_regionsItem;
    QGraphicsPathItem *_grid;

    Clan* _selectedClan;
    Region* _selectedRegion;
    QGraphicsPathItem *_selectionItem;

    QTimer _timer;
    int _ms;
};
#endif // MAINWINDOW_H
