#include <QRandomGenerator>
#include <QTime>
#include <QGraphicsPixmapItem>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphicsviewzoom.h"
/*надо сделать
 *выбор кисти биома и сохранение биома в Region
 *от чего зависит еда на территории
 *цвета клеток
 *
 *Идеи по функционалу клеток
 *модификаторы на атаку/защиту/добычу/перемещение, в виде множителя, но если лучше одно то чуже другое
 *
 *переход
 *набег, вымогтальсто(не заврешает?)
 *торговля или сотрудничество(не заврешают?)
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

    //число клеток
    ui->cellsNumberToAdd->setMinimum(1);
    ui->cellsNumberToAdd->setMaximum(100000);

    //тип добавляемых клеток
    ui->cellsType->addItem("Случайные");
    ui->cellsType->addItem("Собиратели");

    //соединяем слот выбора режима отображения с радио-кнопками
    _displayMode = World::DisplayMode::Сommon;
    connect(ui->commonMode, &QRadioButton::clicked, this, &MainWindow::slotRadioButtons);
    connect(ui->strengthMode, &QRadioButton::clicked, this, &MainWindow::slotRadioButtons);
    connect(ui->foodMode, &QRadioButton::clicked, this, &MainWindow::slotRadioButtons);
    connect(ui->ageMode, &QRadioButton::clicked, this, &MainWindow::slotRadioButtons);

    _world = new World;    
    _selectedCell = nullptr;
    _selectedRegion = nullptr;
    _selectionItem = nullptr;
    _ms = 0;
}

MainWindow::~MainWindow()
{
    disconnect(ui->commonMode, &QRadioButton::clicked, this, &MainWindow::slotRadioButtons);
    disconnect(ui->strengthMode, &QRadioButton::clicked, this, &MainWindow::slotRadioButtons);
    disconnect(ui->foodMode, &QRadioButton::clicked, this, &MainWindow::slotRadioButtons);
    disconnect(ui->ageMode, &QRadioButton::clicked, this, &MainWindow::slotRadioButtons);
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
    _selectedCell = nullptr;
    _selectedRegion = nullptr;
    _selectionItem = nullptr;
    clearInfo();
    ui->cellsNumber->setNum(0);
    ui->stepNumber->setNum(0);

    //создаем пустой мир
    _world = new World(ui->widthWorld->value(), ui->heightWorld->value());
    _scene->setSceneRect(0, 0, _world->width(), _world->height());
    this->addGrid();

    //заполняем мир и загружаем изображения на сцену
    fillWorldWithRegions();
    QImage regionsImage(_world->width(), _world->height(), QImage::Format_ARGB32);
    _world->getRegionsImage(regionsImage, _displayMode);
    _regionsItem =_scene->addPixmap(QPixmap::fromImage(regionsImage));

    QImage cellsImage(_world->width(), _world->height(), QImage::Format_ARGB32);
    _world->getCellsImage(cellsImage, _displayMode);
    _cellsItem =_scene->addPixmap(QPixmap::fromImage(cellsImage));

    setOrderLayers();
    setEnabledWorldChangeInterface(true);
    ui->commonMode->setEnabled(true);
    ui->foodMode->setEnabled(true);
    ui->ageMode->setEnabled(true);
    ui->strengthMode->setEnabled(true);
}


void MainWindow::on_addCells_clicked()
{
    int n = ui->cellsNumberToAdd->value();
    Cell::GenomeInitType mode;
    if (ui->cellsType->currentText() == "Случайные")
        mode = Cell::GenomeInitType::Random;
    if (ui->cellsType->currentText() == "Собиратели")
        mode = Cell::GenomeInitType::Collector;

    //запрашиваем совобдные места
    QList<int> emptySpaces;
    _world->getNumsOfEmptySpaces(emptySpaces);

    //случайно выбираем n или меньше(если n выбрать невозможно) мест
    if (emptySpaces.size() < n)
        n = emptySpaces.size();
    else
        emptySpaces = sample(emptySpaces, n);

    //ставим новые клетки на места
    Cell* cell;
    for (int i = 0; i < n; ++i)
    {
        int x = emptySpaces[i] / _world->height();
        int y = emptySpaces[i] % _world->height();
        cell = new Cell(mode);
        _world->addCell(x, y, cell);
    }

    //загружаем изменения на сцену
    QImage cellsImage(_world->width(), _world->height(), QImage::Format_ARGB32);
    _world->getCellsImage(cellsImage, _displayMode);
    _cellsItem->setPixmap(QPixmap::fromImage(cellsImage));

    //разблокируем старт
    ui->cellsNumber->setNum(ui->cellsNumber->text().toInt() + n);
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
    QImage cellsImage(_world->width(), _world->height(), QImage::Format_ARGB32);
    _world->getCellsImage(cellsImage, _displayMode);
    _cellsItem->setPixmap(QPixmap::fromImage(cellsImage));

    QImage regionsImage(_world->width(), _world->height(), QImage::Format_ARGB32);
    _world->getRegionsImage(regionsImage, _displayMode);
    _regionsItem->setPixmap(QPixmap::fromImage(regionsImage));

    //отслеживаем перемещение выбранной клетки
    //если клетка была удалена, то slotSelectedCellKilled установил _selectedCell = nullptr
    //если _selectedCell == nullptr, то pos == World::CellUndefined
    QPoint pos = _world->getCellPos(_selectedCell);
    if (pos != World::CellUndefined) //Если клетка еще существует
        displaySelection(pos.x(),pos.y());
    else
        _selectedCell = nullptr;

    //выводим информацию о выбранных объектах, числе клеток, номере шага, время итерации
    displayInfo();
    ui->cellsNumber->setNum(_world->cellsNumber());
    ui->stepNumber->setNum(ui->stepNumber->text().toInt() + 1);
    ui->time->setNum(QTime::currentTime().msecsSinceStartOfDay() - _ms);
    _ms = QTime::currentTime().msecsSinceStartOfDay();


    if (ui->stepNumber->text().toInt() % 50 == 0)
        QPixmap::fromImage(cellsImage).save(ui->stepNumber->text() + ".png");
}

void MainWindow::slotSelectedCellKilled()
{
    disconnect(_selectedCell, &Cell::signalKilled, this, &MainWindow::slotSelectedCellKilled);
    _selectedCell = nullptr;
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
        _world->getRegionsImage(image, _displayMode);
        _regionsItem->setPixmap(QPixmap::fromImage(image));
    }
}

void MainWindow::slotMidButton(QGraphicsSceneMouseEvent *mouseEvent)
{
    int x = mouseEvent->scenePos().x();
    int y = mouseEvent->scenePos().y();

    if (_selectedCell)
        disconnect(_selectedCell, &Cell::signalKilled, this, &MainWindow::slotSelectedCellKilled);

    _selectedCell = _world->getCell(x,y);
    _selectedRegion = _world->getRegion(x,y);

    if (_selectedCell)
        connect(_selectedCell, &Cell::signalKilled, this, &MainWindow::slotSelectedCellKilled);

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
    if (_selectedCell)
    {
        uint8_t genom[Cell::_size];
        _selectedCell->getGenom(genom);
        QString str;
        for (int i = 0; i < Cell::_size; ++i)
        {
            str += QString::number(static_cast<int>(genom[i])) + ' ';
            if ((i + 1) % 8 == 0)
                str += '\n';
        }
        ui->genom->setText(str);
        ui->food->setNum(_selectedCell->getFood());
        ui->age->setNum(_selectedCell->getAge());
        ui->strength->setNum(_selectedCell->getStrength());
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
    if (_selectedCell || _selectedRegion)
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
    ui->strength->clear();
    ui->age->clear();
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
    ui->addCells->setEnabled(x);
    ui->cellsNumberToAdd->setEnabled(x);
    ui->cellsType->setEnabled(x);
}

void MainWindow::setOrderLayers()
{
    if (_displayMode == World::DisplayMode::Сommon ||
        _displayMode == World::DisplayMode::Strength ||
        _displayMode == World::DisplayMode::Food ||
        _displayMode == World::DisplayMode::Age)
    {
        _regionsItem->setZValue(static_cast<int>(LayerLevel::Region));
        _cellsItem->setZValue(static_cast<int>(LayerLevel::Cell));
    }

}

void MainWindow::slotRadioButtons()
{
    if (ui->commonMode->isChecked())
        _displayMode = World::DisplayMode::Сommon;
    if (ui->strengthMode->isChecked())
        _displayMode = World::DisplayMode::Strength;
    if (ui->foodMode->isChecked())
        _displayMode = World::DisplayMode::Food;
    if (ui->ageMode->isChecked())
        _displayMode = World::DisplayMode::Age;


    QImage regionsImage(_world->width(), _world->height(), QImage::Format_ARGB32);
    _world->getRegionsImage(regionsImage, _displayMode);
    _regionsItem->setPixmap(QPixmap::fromImage(regionsImage));

    QImage cellsImage(_world->width(), _world->height(), QImage::Format_ARGB32);
    _world->getCellsImage(cellsImage, _displayMode);
    _cellsItem->setPixmap(QPixmap::fromImage(cellsImage));

    setOrderLayers();
}
