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
    //слот нажатия на кнопку создания мира
    void on_createWorld_clicked();

    //слот нажатия на кнопку добавления кланов
    void on_addClans_clicked();

    //слот нажатия на кнопку старт/пауза
    void on_start_clicked();

    //слот запуска геномов, срабатывает по таймеру
    void run();

    /*Если клан был убит, то он будет динамически удален
     *и чтобы _selectedClan не указывал на удаленный участок памяти вызывается этот слот,
     *где _selectedClan станет равным nullptr*/
    void slotSelectedClanKilled();

    //слот изменения биомов
    void slotPainting(QGraphicsSceneMouseEvent *mouseEvent);

    //слот выбора клана и/или региона
    void slotMidButton(QGraphicsSceneMouseEvent *mouseEvent);

    //случайная подпоследовательность длинной count, без повторений
    QVector<int> sample(QVector<int>& seq, int count);
private:
    //добавляет сетку на сцену
    void addGrid();

    // отображает информацию о выбранных территории и клане
    void displayInfo();

    // отображает рамку по указанным координтам
    void displaySelection(int x, int y);

    // очищает информацию о выбранных территории и клане
    void clearInfo();

    //заполняет мир регионами
    void fillWorldWithRegions();

    void setEnabledWorldChangeInterface(bool x);
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
