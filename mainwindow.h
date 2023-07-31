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
private:

    /*Класс нужен затем чтобы делегировать на него полномочия по
     * отображению рамки выделения и информации о выбранных объектах*/
    class SelectionUI
    {
    public:
        SelectionUI();

        // стирает информацию о выделении, обнуляет поля
        void clear();

        // возвращет указатель на выбранный регион
        Region *region();

        // запоминает объекты по указанным координтам в указанном мире
        void select(int x, int y, World *world);

        // отображает рамку по указанным координтам
        void displaySelection(int x, int y, PaintableScene *scene);

        // отображает информацию о территории и клане на ui
        void displayInfo(Ui::MainWindow *ui);

        //обновляет положение клана и оборжаает рамку выделения
        void updateClanPos(World *world, PaintableScene *scene);
    private:
        Clan* _clan;
        Region* _region;
        QGraphicsPathItem *_selectionItem;
    };


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum class LayerLevel{Region = 0, Clan = 1, Grid = 2, HighlightSelection = 3};
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
    World *_world;

    QGraphicsPixmapItem *_clansItem, *_regionsItem;
    QGraphicsPathItem *_grid;
    SelectionUI _selectation;

    int _clansNumber;
    QTimer _timer;
    int _ms;
};
#endif // MAINWINDOW_H
