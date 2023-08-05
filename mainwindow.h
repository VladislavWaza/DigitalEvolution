#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include "paintablescene.h"
#include "world.h"
#include "region.h"
#include "cell.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum class LayerLevel{Region = 0, Cell = 1, Grid = 2, HighlightSelection = 3};
private slots:
    //слот нажатия на кнопку создания мира
    void on_createWorld_clicked();

    //слот нажатия на кнопку добавления клеток
    void on_addCells_clicked();

    //слот нажатия на кнопку старт/пауза
    void on_start_clicked();

    //слот запуска геномов, срабатывает по таймеру
    void run();

    /*Если клетка была убита, то она будет динамически удалена
     *и чтобы _selectedCell не указывал на удаленный участок памяти вызывается этот слот,
     *где _selectedCell станет равным nullptr*/
    void slotSelectedCellKilled();

    //слот изменения биомов
    void slotPainting(QGraphicsSceneMouseEvent *mouseEvent);

    //слот выбора клана и/или региона
    void slotMidButton(QGraphicsSceneMouseEvent *mouseEvent);

    void slotRadioButtons();

private:
    //случайная подпоследовательность длинной count, без повторений
    QVector<int> sample(QVector<int>& seq, int count);

    //добавляет сетку на сцену
    void addGrid();

    // отображает информацию о выбранном регионе и клетке
    void displayInfo();

    // отображает рамку по указанным координтам
    void displaySelection(int x, int y);

    // очищает информацию о выбранном регионе и клетке
    void clearInfo();

    //заполняет мир регионами
    void fillWorldWithRegions();

    void setEnabledWorldChangeInterface(bool x);

    void setOrderLayers();


    Ui::MainWindow *ui;
    PaintableScene *_scene;
    World *_world;
    World::DisplayMode _displayMode;

    QGraphicsPixmapItem *_cellsItem, *_regionsItem;
    QGraphicsPathItem *_grid;

    Cell* _selectedCell;
    Region* _selectedRegion;
    QGraphicsPathItem *_selectionItem;

    QTimer _timer;
    int _ms;
};
#endif // MAINWINDOW_H
