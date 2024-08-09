#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphicsviewzoom.h"

#include <QTime>
#include <QGraphicsPixmapItem>


using DigitalEvolution::WorldSimulation;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(std::make_unique<Ui::MainWindow>())
{
    m_ui->setupUi(this);

    //соединяем таймер и выполнение геномов
    connect(&m_timer, &QTimer::timeout, this, &MainWindow::run);

    //добавляем зум
    GraphicsViewZoom* zoom = new GraphicsViewZoom(m_ui->graphicsView);
    zoom->setModifiers(Qt::NoModifier);

    //настройка graphicsView
    m_ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    m_ui->graphicsView->setRenderHint(QPainter::SmoothPixmapTransform);

    //создаем сцену
    m_scene = std::make_unique<QGraphicsScene>(this);
    m_ui->graphicsView->setScene(m_scene.get());

    //параметры мира
    m_ui->heightWorld->setMaximum(10000);
    m_ui->widthWorld->setMaximum(10000);
    m_ui->heightWorld->setMinimum(50);
    m_ui->widthWorld->setMinimum(50);

    m_ui->cellsNumberToAdd->setMinimum(1);
    m_ui->cellsNumberToAdd->setMaximum(10000000);

    m_ui->detailLevel->addItem("1 пиксель на клетку");
    m_ui->detailLevel->addItem("9 пикселей на клетку");
    m_ui->detailLevel->addItem("25 пикселей на клетку");

    m_ms = 0;
}

MainWindow::~MainWindow()
{
}


void MainWindow::on_createWorld_clicked()
{
    m_scene->clear();
    m_ui->stepNumber->setNum(0);

    WorldSimulation::CellsDetailLevel cellsDetailLevel = WorldSimulation::CellsDetailLevel::OnePixel;
    if (m_ui->detailLevel->currentText() == "1 пиксель на клетку")
        cellsDetailLevel = WorldSimulation::CellsDetailLevel::OnePixel;
    else if (m_ui->detailLevel->currentText() == "9 пикселей на клетку")
        cellsDetailLevel = WorldSimulation::CellsDetailLevel::Square3x3;
    else if (m_ui->detailLevel->currentText() == "25 пикселей на клетку")
        cellsDetailLevel = WorldSimulation::CellsDetailLevel::Square5x5;

    m_world = std::make_unique<WorldSimulation>(m_ui->widthWorld->value(), m_ui->heightWorld->value(), cellsDetailLevel);
    m_scene->setSceneRect(0, 0, m_world->imageWidth(), m_world->imageHeight());
    this->addGrid();

    m_pixmapItem = m_scene->addPixmap(QPixmap::fromImage(m_world->getImage()));

    setEnabledWorldChangeInterface(true);
    m_ui->start->setEnabled(true);
}

void MainWindow::on_start_clicked()
{
    if (m_ui->start->text() == "Запустить")
    {
        setEnabledWorldChangeInterface(false);
        m_ui->start->setText("Пауза");
        m_timer.start(k_timerDelay);
    }
    else
    {
        m_timer.stop();
        setEnabledWorldChangeInterface(true);
        m_ui->start->setText("Запустить");
    }
}

void MainWindow::run()
{
    auto ms = QTime::currentTime().msecsSinceStartOfDay();
    m_world->run();
    qDebug() << "run(): " << QTime::currentTime().msecsSinceStartOfDay() - ms;
    ms = QTime::currentTime().msecsSinceStartOfDay();
    m_pixmapItem->setPixmap(QPixmap::fromImage(m_world->getImage()));
    qDebug() << "setPixmap(): " << QTime::currentTime().msecsSinceStartOfDay() - ms;

    m_ui->stepNumber->setText(QString::number(m_world->stepsNumber()));
    m_ui->cellsNumber->setText(QString::number(m_world->cellsCount()));
    m_ui->time->setNum(QTime::currentTime().msecsSinceStartOfDay() - m_ms);
    m_ms = QTime::currentTime().msecsSinceStartOfDay();
}

void MainWindow::addGrid()
{
    QPainterPath gridPath;
    const size_t w = m_world->imageWidth();
    const size_t h = m_world->imageHeight();
    const size_t step = m_world->cellSize();
    for (size_t x = 0; x <= w; x+=step)
    {
        gridPath.moveTo(x, 0);
        gridPath.lineTo(x, h);

    }
    for (size_t y = 0; y <= h; y+=step)
    {
        gridPath.moveTo(0, y);
        gridPath.lineTo(w, y);
    }
    m_gridItem = m_scene->addPath(gridPath, QPen(Qt::black, 0.02));
    m_gridItem->setZValue(static_cast<int>(LayerLevel::Grid));
}

void MainWindow::setEnabledWorldChangeInterface(bool x)
{
    m_ui->createWorld->setEnabled(x);
    m_ui->heightWorld->setEnabled(x);
    m_ui->widthWorld->setEnabled(x);
    m_ui->label->setEnabled(x);
    m_ui->label_2->setEnabled(x);
    m_ui->addCells->setEnabled(x);
    m_ui->cellsNumberToAdd->setEnabled(x);
}

void MainWindow::on_addCells_clicked()
{
    auto ms = QTime::currentTime().msecsSinceStartOfDay();
    m_world->addCells(m_ui->cellsNumberToAdd->value());
    qDebug() << "addCells(): " << QTime::currentTime().msecsSinceStartOfDay() - ms;
    m_pixmapItem->setPixmap(QPixmap::fromImage(m_world->getImage()));
    m_ui->cellsNumber->setNum(static_cast<int>(m_world->cellsCount()));
}

