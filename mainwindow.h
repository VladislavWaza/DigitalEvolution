#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "paintablescene.h"
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

private slots:
    void on_createWorld_clicked();

    void slotPainting(QGraphicsSceneMouseEvent *mouseEvent);

    void on_addClans_clicked();

    void on_start_clicked();

    void run();
private:
    Ui::MainWindow *ui;
    PaintableScene *_scene;
    int _heightWorld, _widthWorld;
    QList<Region*> _regions;
    QList<Clan*> _clans;
    QTimer _timer;

};
#endif // MAINWINDOW_H
