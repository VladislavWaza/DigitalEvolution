#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "worldsimulation.h"

#include <QMainWindow>
#include <QTimer>
#include <QGraphicsScene>

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    enum class LayerLevel {Cell = 0, Grid = 1};
private slots:
    void on_createWorld_clicked();

    void on_start_clicked();

    void run();

    void on_addCells_clicked();

private:
    void addGrid();

    void setEnabledWorldChangeInterface(bool x);

    std::unique_ptr<Ui::MainWindow> m_ui;
    std::unique_ptr<QGraphicsScene> m_scene;
    std::unique_ptr<DigitalEvolution::WorldSimulation> m_world;

    QGraphicsPathItem *m_gridItem = nullptr;
    QGraphicsPixmapItem *m_pixmapItem = nullptr;

    QTimer m_timer;
    int m_ms = 0;
    const int k_timerDelay = 20;
};
#endif // MAINWINDOW_H
