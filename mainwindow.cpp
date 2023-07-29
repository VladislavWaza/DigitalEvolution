#include <QRandomGenerator>
#include <QTime>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphicsviewzoom.h"
/*надо сделать
 *выбор кисти биома и сохранение биома в Region
 *передаелть алгортим генерации места клана при добавлении
 *ориетация перемещения на энумах
 *ускорить итерации
 *закольцевать мир
 *интерфейс отобржения шагов и блокировка части интерфейса при выполнении
 *QGraphicsItem::type()
 *QImage вместо Items bits()
 *сетка - drawBackground
 *
 */


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _ms = 0;
    //соединяем таймер и выполнение геномов
    connect(&_timer, &QTimer::timeout, this, &MainWindow::run);

    //добавляем зум
    GraphicsViewZoom* zoom = new GraphicsViewZoom(ui->graphicsView);
    zoom->setModifiers(Qt::NoModifier);
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    //ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    //ui->graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    //ui->graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    //создаем сцену и соединяем со слотом рисовния
    _scene = new PaintableScene(this);
    ui->graphicsView->setScene(_scene);
    connect(_scene, &PaintableScene::signalPainting, this, &MainWindow::slotPainting);

    //параметры мира
    ui->heightWorld->setMaximum(1000);
    ui->widthWorld->setMaximum(1000);
    ui->heightWorld->setMinimum(50);
    ui->widthWorld->setMinimum(50);

    //параметры кисти
    ui->brushDiameter->setMinimum(1);
    ui->brushDiameter->setMaximum(500);
    ui->brushBiom->addItem("Степь");
    ui->brushBiom->addItem("Лес");

    //число кланов
    ui->numberClans->setMinimum(1);
    ui->numberClans->setMaximum(10000);

    _world = new World();
}

MainWindow::~MainWindow()
{
    disconnect(_scene, &PaintableScene::signalPainting, this, &MainWindow::slotPainting);
    delete _world;
    delete ui;
    delete _scene;
}


void MainWindow::on_createWorld_clicked()
{
    delete _world;
    _regions.clear();
    _clans.clear();
    _scene->clear();

    //создаем пустой мир
    _heightWorld = ui->heightWorld->value();
    _widthWorld = ui->widthWorld->value();
    _world = new World(_widthWorld, _heightWorld);
    _scene->setSceneRect(0, 0, _widthWorld * 5, _heightWorld * 5);
    _scene->setBackgroundBrush(Qt::blue);
    ui->label_5->setNum(0);

    //разблокируем часть интерфейса
    ui->brushDiameter->setEnabled(true);
    ui->label_3->setEnabled(true);
    ui->brushBiom->setEnabled(true);
    ui->label_4->setEnabled(true);
    ui->addClans->setEnabled(true);
    ui->numberClans->setEnabled(true);

    //заполняем мир
    QBrush brush(Qt::gray);
    QPen pen;
    pen.setWidthF(0.25);
    Region* item;
    for (int i = 0; i < _widthWorld; ++i)
    {
        for (int j = 0; j < _heightWorld; ++j)
        {
            item = new Region(i*5, j*5, 5, 5);
            item->setBrush(brush);
            item->setPen(pen);
            _regions.append(item);
            _scene->addItem(item);
        }
    }
}

void MainWindow::slotPainting(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (ui->brushDiameter->isEnabled())
    {
        QPainterPath circle;
        circle.addEllipse(mouseEvent->scenePos(), ui->brushDiameter->value()/2, ui->brushDiameter->value()/2);
        QList<QGraphicsItem*> items = _scene->items(circle, Qt::IntersectsItemShape, Qt::DescendingOrder);
        for (QGraphicsItem* item : items)
        {
            if (item->zValue() == 0)
            {
                //очень не безопасно, если item не наследник QAbstractGraphicsShapeItem
                QAbstractGraphicsShapeItem *rect = static_cast<QAbstractGraphicsShapeItem*>(item);
                rect->setBrush(Qt::white);
            }
        }
    }
}

void MainWindow::on_addClans_clicked()
{
    int n = ui->numberClans->value();
    QBrush brush(Qt::yellow);
    QPen pen;
    pen.setWidthF(0.25);
    Clan* item;
    for (int i = 0; i < n; ++i)
    {
        if (_clans.size() == _widthWorld * _heightWorld)
            break;

        item = new Clan();

        //плохой алгортим
        int x = QRandomGenerator::system()->bounded(_widthWorld);
        int y = QRandomGenerator::system()->bounded(_heightWorld);
        while (_scene->items(QPointF(5*x + 1, 5*y + 1), Qt::IntersectsItemShape, Qt::DescendingOrder).size() > 1)
        {
            x = QRandomGenerator::system()->bounded(_widthWorld);
            y = QRandomGenerator::system()->bounded(_heightWorld);
        }
        item->setRect(5*x, 5*y, 5, 5);
        item->setBrush(brush);
        item->setPen(pen);
        item->setZValue(1); //кланы будут на слой выше чем регионы

        _world->addItem(x, y, item);
        _clans.append(item);
        _scene->addItem(item);
    }

    ui->start->setEnabled(true);
}

void MainWindow::on_start_clicked()
{
    if (ui->start->text() == "Запустить")
    {
        ui->start->setText("Пауза");
        _timer.start(20);
    }
    else
    {
        _timer.stop();
        ui->start->setText("Запустить");
    }
}

void MainWindow::run()
{
    if (_ms == 0)
        _ms = QTime::currentTime().msecsSinceStartOfDay();
    _world->run();
    ui->label_5->setNum(ui->label_5->text().toInt() + 1);
    qDebug() << QTime::currentTime().msecsSinceStartOfDay() - _ms;
    _ms = QTime::currentTime().msecsSinceStartOfDay();
}
