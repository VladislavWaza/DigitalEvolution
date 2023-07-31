#include <QRandomGenerator>
#include <QTime>
#include <QGraphicsPixmapItem>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphicsviewzoom.h"
/*надо сделать
 *выбор кисти биома и сохранение биома в Region
 *интерфейс отобржения шагов и блокировка части интерфейса при выполнении
 *убрать Image из World
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

    //соединяем таймер и выполнение геномов
    connect(&_timer, &QTimer::timeout, this, &MainWindow::run);

    //добавляем зум
    GraphicsViewZoom* zoom = new GraphicsViewZoom(ui->graphicsView);
    zoom->setModifiers(Qt::NoModifier);

    //настройка graphicsView
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsView->setRenderHint(QPainter::SmoothPixmapTransform);

    //создаем сцену и соединяем со слотом рисовния и нажатия средней клавиши мыши
    _scene = new PaintableScene(this);
    ui->graphicsView->setScene(_scene);
    connect(_scene, &PaintableScene::signalPainting, this, &MainWindow::slotPainting);
    connect(_scene, &PaintableScene::signalMidButton, this, &MainWindow::slotMidButton);

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
    ui->numberClans->setMaximum(100000);

    _world = new World;
    _selectedClan = nullptr;
    _selectedRegion = nullptr;
    _selectionItem = nullptr;
    _ms = 0;
}

MainWindow::~MainWindow()
{
    disconnect(_scene, &PaintableScene::signalPainting, this, &MainWindow::slotPainting);
    disconnect(_scene, &PaintableScene::signalMidButton, this, &MainWindow::slotMidButton);
    delete _world;
    delete ui;
    delete _scene;
}


void MainWindow::on_createWorld_clicked()
{
    delete _world;
    _scene->clear();


    _selectedClan = nullptr;
    _selectedRegion = nullptr;
    _selectionItem = nullptr;
    ui->label->clear();

    //создаем пустой мир
    _world = new World(ui->widthWorld->value(), ui->heightWorld->value());
    _scene->setSceneRect(0, 0, _world->width(), _world->height());
    ui->clansCounter->setNum(0);
    this->addGrid();

    //разблокируем часть интерфейса
    ui->stepNumber->setNum(0);
    ui->brushDiameter->setEnabled(true);
    ui->label_3->setEnabled(true);
    ui->brushBiom->setEnabled(true);
    ui->label_4->setEnabled(true);
    ui->addClans->setEnabled(true);
    ui->numberClans->setEnabled(true);

    //заполняем мир
    QImage regionsImage(_world->width(), _world->height(), QImage::Format_ARGB32);
    QImage clansImage(_world->width(), _world->height(), QImage::Format_ARGB32);
    Region* region;
    for (int i = 0; i < _world->width(); ++i)
    {
        for (int j = 0; j < _world->height(); ++j)
        {
            region = new Region;
            _world->setRegion(i,j,region);
            regionsImage.setPixelColor(i,j, Qt::gray);
            clansImage.setPixelColor(i,j, QColor(0,0,0,0));
        }
    }

    //помещаем изображения на сцену
    _regionsItem =_scene->addPixmap(QPixmap::fromImage(regionsImage));
    _clansItem =_scene->addPixmap(QPixmap::fromImage(clansImage));
    _clansItem->setZValue(static_cast<int>(LayerLevel::Clan)); //кланы располагаем на слой выше
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
        ui->clansCounter->setNum(ui->clansCounter->text().toInt() + 1);
    }
    _clansItem->setPixmap(QPixmap::fromImage(clansImage));
    ui->start->setEnabled(true);
}

void MainWindow::on_start_clicked()
{
    if (ui->start->text() == "Запустить")
    {
        ui->start->setText("Пауза");
        ui->createWorld->setEnabled(false);
        ui->heightWorld->setEnabled(false);
        ui->widthWorld->setEnabled(false);
        ui->label->setEnabled(false);
        ui->label_2->setEnabled(false);
        ui->brushDiameter->setEnabled(false);
        ui->label_3->setEnabled(false);
        ui->brushBiom->setEnabled(false);
        ui->label_4->setEnabled(false);
        ui->addClans->setEnabled(false);
        ui->numberClans->setEnabled(false);
        _timer.start(20);
    }
    else
    {
        ui->createWorld->setEnabled(true);
        ui->heightWorld->setEnabled(true);
        ui->widthWorld->setEnabled(true);
        ui->label->setEnabled(true);
        ui->label_2->setEnabled(true);
        ui->brushDiameter->setEnabled(true);
        ui->label_3->setEnabled(true);
        ui->brushBiom->setEnabled(true);
        ui->label_4->setEnabled(true);
        ui->addClans->setEnabled(true);
        ui->numberClans->setEnabled(true);
        _timer.stop();
        ui->start->setText("Запустить");
    }
}

void MainWindow::run()
{
    if (_ms == 0)
        _ms = QTime::currentTime().msecsSinceStartOfDay();

    QImage clansImage = _clansItem->pixmap().toImage();
    clansImage.convertTo(QImage::Format_ARGB32);
    _world->run(clansImage);
    _clansItem->setPixmap(QPixmap::fromImage(clansImage));

    QPoint pos = _world->getClanPos(_selectedClan);
    if (pos != World::ClanUndefined) //Если _clan еще существует
        displaySelection(pos.x(),pos.y());
    else //Если клан был удален или _clan == nullptr
        _selectedClan = nullptr;

    displayInfo();
    ui->clansCounter->setNum(_world->clansNumber());
    ui->stepNumber->setNum(ui->stepNumber->text().toInt() + 1);
    //qDebug() << QTime::currentTime().msecsSinceStartOfDay() - _ms;
    //_ms = QTime::currentTime().msecsSinceStartOfDay();
}

void MainWindow::slotSelectedClanKilled()
{
    disconnect(_selectedClan, &Clan::signalKilled, this, &MainWindow::slotSelectedClanKilled);
    _selectedClan = nullptr;
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
                {
                    image.setPixelColor(x, y, Qt::white);
                    Region *region = _world->getRegion(x,y);
                    //region->setBiom(biom);
                    if (_selectedRegion == region)
                        displayInfo();
                }
            }
        }
        _regionsItem->setPixmap(QPixmap::fromImage(image));
    }
}

void MainWindow::slotMidButton(QGraphicsSceneMouseEvent *mouseEvent)
{
    int x = mouseEvent->scenePos().x();
    int y = mouseEvent->scenePos().y();
    if (_selectedClan)
        disconnect(_selectedClan, &Clan::signalKilled, this, &MainWindow::slotSelectedClanKilled);
    _selectedClan = _world->getClan(x,y);
    _selectedRegion = _world->getRegion(x,y);
    if (_selectedClan)
        connect(_selectedClan, &Clan::signalKilled, this, &MainWindow::slotSelectedClanKilled);
    displaySelection(x,y);
    displayInfo();
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

void MainWindow::addGrid()
{
    QPainterPath gridPath;
    for (int x = 0; x <= _world->width(); ++x)
    {
        gridPath.moveTo(x,0);
        gridPath.lineTo(x,_world->height());

    }
    for (int y = 0; y <= _world->height(); ++y)
    {
        gridPath.moveTo(0,y);
        gridPath.lineTo(_world->width(),y);
    }
    _grid = _scene->addPath(gridPath, QPen(Qt::black, 0.02));
    _grid->setZValue(static_cast<int>(LayerLevel::Grid));
}

void MainWindow::displayInfo()
{
    if (_selectedClan)
    {
        uint8_t genom[Clan::_size];
        _selectedClan->getGenom(genom);
        QString str;
        for (int i = 0; i < Clan::_size; ++i)
            str += QString::number(static_cast<int>(genom[i])) + ' ';
        ui->genom->setText(str);
    }
    else
        ui->genom->clear();
}

void MainWindow::displaySelection(int x, int y)
{
    if (_selectionItem)
    {
        _scene->removeItem(_selectionItem);
        _selectionItem = nullptr;
    }
    if (_selectedClan || _selectedRegion)
    {
        QPainterPath path;
        path.addRect(x,y,1,1);
        _selectionItem = _scene->addPath(path, QPen(Qt::white, 0.1));
        _selectionItem->setZValue(static_cast<int>(MainWindow::LayerLevel::HighlightSelection));
    }
}
