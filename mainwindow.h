#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QGraphicsScene>

#include <vector>

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
    //слот нажатия на кнопку создания мира
    void on_createWorld_clicked();

    //слот нажатия на кнопку старт/пауза
    void on_start_clicked();

    //слот запуска геномов, срабатывает по таймеру
    void run();

private:
    //добавляет сетку на сцену
    void addGrid();

    void setEnabledWorldChangeInterface(bool x);

    Ui::MainWindow *m_ui;
    QGraphicsScene *m_scene  = nullptr;

    QGraphicsPathItem *m_grid = nullptr;
    QGraphicsPixmapItem *m_pixmap  = nullptr;
    std::vector<QGraphicsEllipseItem*> m_ellipseItems;

    QTimer m_timer;
    int m_ms = 0;
    qsizetype m_width = 0;
    qsizetype m_height = 0;
    qsizetype m_timerDelay = 20;
};
#endif // MAINWINDOW_H
