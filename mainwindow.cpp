#include <QRandomGenerator>
#include <QTime>
#include <QGraphicsPixmapItem>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphicsviewzoom.h"
/*надо сделать
 *выбор кисти биома и сохранение биома в Region
 *тепловые карты
 *от чего зависит еда на территории
 *цвета кланов
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
    ui->clansNumberToAdd->setMinimum(1);
    ui->clansNumberToAdd->setMaximum(100000);

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
    //очищаем мир, сцену и интерфейс
    delete _world;
    _scene->clear();
    _selectedClan = nullptr;
    _selectedRegion = nullptr;
    _selectionItem = nullptr;
    clearInfo();
    ui->clansNumber->setNum(0);
    ui->stepNumber->setNum(0);

    //создаем пустой мир
    _world = new World(ui->widthWorld->value(), ui->heightWorld->value());
    _scene->setSceneRect(0, 0, _world->width(), _world->height());
    this->addGrid();

    //разблокируем интерфейс связанный с изменением биомов и добавлением кланов
    ui->brushDiameter->setEnabled(true);
    ui->label_3->setEnabled(true);
    ui->brushBiom->setEnabled(true);
    ui->label_4->setEnabled(true);
    ui->addClans->setEnabled(true);
    ui->clansNumberToAdd->setEnabled(true);

    //заполняем мир и загружаем изображения на сцену
    fillWorldWithRegions();
    QImage regionsImage(_world->width(), _world->height(), QImage::Format_ARGB32);
    _world->getRegionsImage(regionsImage);
    _regionsItem =_scene->addPixmap(QPixmap::fromImage(regionsImage));
    _regionsItem->setZValue(static_cast<int>(LayerLevel::Region));

    QImage clansImage(_world->width(), _world->height(), QImage::Format_ARGB32);
    _world->getClansImage(clansImage);
    _clansItem =_scene->addPixmap(QPixmap::fromImage(clansImage));
    _clansItem->setZValue(static_cast<int>(LayerLevel::Clan));


}


void MainWindow::on_addClans_clicked()
{
    int n = ui->clansNumberToAdd->value();

    //запрашиваем совобдные места
    QList<int> emptySpaces;
    _world->getNumsOfEmptySpaces(emptySpaces);

    //случайно выбираем n или меньше(если n выбрать невозможно) мест
    if (emptySpaces.size() < n)
        n = emptySpaces.size();
    else
        emptySpaces = sample(emptySpaces, n);

    //ставим новые кланы на места
    Clan* clan;
    for (int i = 0; i < n; ++i)
    {
        int x = emptySpaces[i] / _world->height();
        int y = emptySpaces[i] % _world->height();
        clan = new Clan;
        _world->addClan(x, y, clan);
    }

    //загружаем изменения на сцену
    QImage clansImage(_world->width(), _world->height(), QImage::Format_ARGB32);
    _world->getClansImage(clansImage);
    _clansItem->setPixmap(QPixmap::fromImage(clansImage));

    //разблокируем старт
    ui->clansNumber->setNum(ui->clansNumber->text().toInt() + n);
    ui->start->setEnabled(true);
}

void MainWindow::on_start_clicked()
{
    if (ui->start->text() == "Запустить")
    {
        setEnabledWorldChangeInterface(false);
        ui->start->setText("Пауза");     
        _timer.start(20);
    }
    else
    {
        _timer.stop();
        setEnabledWorldChangeInterface(true);
        ui->start->setText("Запустить");
    }
}

void MainWindow::run()
{
    //запускаем геномы, принимаем изменения в изображении
    _world->run();
    QImage clansImage(_world->width(), _world->height(), QImage::Format_ARGB32);
    _world->getClansImage(clansImage);
    _clansItem->setPixmap(QPixmap::fromImage(clansImage));

    //отслеживаем перемещение выбранного клана
    //если клан был удален, то slotSelectedClanKilled установил _selectedClan = nullptr
    //если _selectedClan == nullptr, то pos == World::ClanUndefined
    QPoint pos = _world->getClanPos(_selectedClan);
    if (pos != World::ClanUndefined) //Если _clan еще существует
        displaySelection(pos.x(),pos.y());
    else
        _selectedClan = nullptr;

    //выводим информацию о выбранных объектах, числе кланов, номере шага, время итерации
    displayInfo();
    ui->clansNumber->setNum(_world->clansNumber());
    ui->stepNumber->setNum(ui->stepNumber->text().toInt() + 1);
    ui->time->setNum(QTime::currentTime().msecsSinceStartOfDay() - _ms);
    _ms = QTime::currentTime().msecsSinceStartOfDay();
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
        QPainterPath circle;
        circle.addEllipse(mouseEvent->scenePos(), ui->brushDiameter->value()/2, ui->brushDiameter->value()/2);
        for (int x = 0; x < _world->width(); ++x)
        {
            for (int y = 0; y < _world->height(); ++y)
            {
                if (circle.contains(QPointF(x, y)))
                {
                    Region *region = _world->getRegion(x,y);
                    region->setColor(Qt::yellow);
                    if (_selectedRegion == region)
                        displayInfo();
                }
            }
        }
        QImage image(_world->width(), _world->height(), QImage::Format_ARGB32);
        _world->getRegionsImage(image);
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
        ui->food->setNum(_selectedClan->getFood());
    }
    else
        clearInfo();
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

void MainWindow::clearInfo()
{
    ui->genom->clear();
    ui->food->clear();
}

void MainWindow::fillWorldWithRegions()
{
    Region* region;
    for (int i = 0; i < _world->width(); ++i)
    {
        for (int j = 0; j < _world->height(); ++j)
        {
            region = new Region;
            _world->setRegion(i,j,region);
            region->setColor(Qt::gray);
        }
    }
}

void MainWindow::setEnabledWorldChangeInterface(bool x)
{
    ui->createWorld->setEnabled(x);
    ui->heightWorld->setEnabled(x);
    ui->widthWorld->setEnabled(x);
    ui->label->setEnabled(x);
    ui->label_2->setEnabled(x);
    ui->brushDiameter->setEnabled(x);
    ui->label_3->setEnabled(x);
    ui->brushBiom->setEnabled(x);
    ui->label_4->setEnabled(x);
    ui->addClans->setEnabled(x);
    ui->clansNumberToAdd->setEnabled(x);
}
