#include <QRandomGenerator>
#include <QTime>
#include <QGraphicsPixmapItem>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphicsviewzoom.h"
/*надо сделать
 *выбор кисти биома и сохранение биома в Region
 *ускорить итерации
 *интерфейс отобржения шагов и блокировка части интерфейса при выполнении
 *сетка - drawBackground
 *блокирование операций над миром вне паузы
 *переписать slotPainting
 *
 *
 *Идеи по функционалу клана
 *модификаторы на атаку/защиту/добычу/перемещение, в виде множителя, но если лучше одно то чуже другое
 *свой/чужой?
 *
 *переход
 *набег, вымогтальсто(не заврешает?)
 *торговля или сотрудничество(не заврешают?)
 *собирательстов или охота
 *
 *
 *Идеи по функционалу регионов
 *прибрежные районы лучше
 *истощение
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

    //настройка graphicsView
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsView->setRenderHint(QPainter::SmoothPixmapTransform);

    //создаем сцену и соединяем со слотом рисовния и нажатия средней клавиши
    _scene = new PaintableScene(this);
    ui->graphicsView->setScene(_scene);
    connect(_scene, &PaintableScene::signalPainting, this, &MainWindow::slotPainting);
    connect(_scene, &PaintableScene::signalMidButton, this, &MainWindow::slotMidButton);

    //параметры мира
    ui->heightWorld->setMaximum(10000);
    ui->widthWorld->setMaximum(10000);
    ui->heightWorld->setMinimum(50);
    ui->widthWorld->setMinimum(50);

    //параметры кисти
    ui->brushDiameter->setMinimum(1);
    ui->brushDiameter->setMaximum(500);
    ui->brushBiom->addItem("Степь");
    ui->brushBiom->addItem("Лес");

    //число кланов
    ui->numberClans->setMinimum(1);
    ui->numberClans->setMaximum(5000000);

    _world = new World;
    _clansItem = new QGraphicsPixmapItem;
    _regionsItem = new QGraphicsPixmapItem;
}

MainWindow::~MainWindow()
{
    disconnect(_scene, &PaintableScene::signalPainting, this, &MainWindow::slotPainting);
    disconnect(_scene, &PaintableScene::signalMidButton, this, &MainWindow::slotMidButton);
    delete _world;
    delete _clansItem;
    delete _regionsItem;
    delete ui;
    delete _scene;
}


void MainWindow::on_createWorld_clicked()
{
    delete _clansItem;
    delete _regionsItem;
    delete _world;
    _scene->clear();

    //создаем пустой мир
    _world = new World(ui->widthWorld->value(), ui->heightWorld->value());
    _scene->setSceneRect(0, 0, _world->width(), _world->height());
    _clansNumber = 0;

    //разблокируем часть интерфейса
    ui->label_5->setNum(0);
    ui->brushDiameter->setEnabled(true);
    ui->label_3->setEnabled(true);
    ui->brushBiom->setEnabled(true);
    ui->label_4->setEnabled(true);
    ui->addClans->setEnabled(true);
    ui->numberClans->setEnabled(true);

    //заполняем мир
    QImage regionsImage(_world->width(), _world->height(), QImage::Format_ARGB32);
    QImage clansImage(_world->width(), _world->height(), QImage::Format_ARGB32);
    for (int i = 0; i < _world->width(); ++i)
    {
        for (int j = 0; j < _world->height(); ++j)
        {
            regionsImage.setPixelColor(i,j, Qt::gray);
            clansImage.setPixelColor(i,j, QColor(0,0,0,0));
        }
    }

    //помещаем изображения на сцену
    _regionsItem =_scene->addPixmap(QPixmap::fromImage(regionsImage));
    _clansItem =_scene->addPixmap(QPixmap::fromImage(clansImage));
    _clansItem->setZValue(1); //кланы располагаем на слой выше
}


void MainWindow::on_addClans_clicked()
{
    int n = ui->numberClans->value();

    //запрашиваем совобдные места
    QList<int> emptySpaces;
    _world->getNumsOfEmptySpaces(emptySpaces);

    if (emptySpaces.size() < n)
        n = emptySpaces.size();
    else
        emptySpaces = sample(emptySpaces, n); //случайно выбираем n мест

    QImage clansImage = _clansItem->pixmap().toImage();

    Clan* clan;
    for (int i = 0; i < n; ++i)
    {
        int x = emptySpaces[i] / _world->height();
        int y = emptySpaces[i] % _world->height();
        clansImage.setPixelColor(x,y,Qt::darkCyan);
        clan = new Clan;
        _world->addClan(x, y, clan);
        ++_clansNumber;
    }
    _clansItem->setPixmap(QPixmap::fromImage(clansImage));
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
    QImage clansImage = _clansItem->pixmap().toImage();
    _world->run(clansImage);
    _clansItem->setPixmap(QPixmap::fromImage(clansImage));
    ui->label_5->setNum(ui->label_5->text().toInt() + 1);
    qDebug() << QTime::currentTime().msecsSinceStartOfDay() - _ms;
    _ms = QTime::currentTime().msecsSinceStartOfDay();
}

void MainWindow::slotPainting(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (ui->brushDiameter->isEnabled())
    {
        QImage image = _regionsItem->pixmap().toImage();
        QPainterPath circle;
        circle.addEllipse(mouseEvent->scenePos(), ui->brushDiameter->value()/2, ui->brushDiameter->value()/2);
        for (int x = 0; x < image.width(); ++x)
        {
            for (int y = 0; y < image.height(); ++y)
            {
                if (circle.contains(QPointF(x, y)))
                    image.setPixelColor(x, y, Qt::white);
            }
        }
        _regionsItem->setPixmap(QPixmap::fromImage(image));
    }
}

void MainWindow::slotMidButton(QGraphicsSceneMouseEvent *mouseEvent)
{
    int x = mouseEvent->scenePos().x();
    int y = mouseEvent->scenePos().y();
    qDebug() << x << y;

}

QVector<int> MainWindow::sample(QVector<int> &seq, int count)
{
    QVector<int> result;
    if (count < 0)
        return result;
    int randVal;
    for (int i = 0; i < count; ++i)
    {
        randVal = QRandomGenerator::system()->bounded(seq.size()-i);
        result.append(seq[randVal]);
        seq.swapItemsAt(randVal, seq.size()-i-1);
    }
    return result;
}
