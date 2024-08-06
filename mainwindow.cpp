#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphicsviewzoom.h"

#include <QRandomGenerator>
#include <QTime>
#include <QGraphicsPixmapItem>

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

    m_ms = 0;
}

MainWindow::~MainWindow()
{
}


void MainWindow::on_createWorld_clicked()
{
    m_scene->clear();
    m_ui->stepNumber->setNum(0);

    m_width = m_ui->widthWorld->value();
    m_height = m_ui->heightWorld->value();
    m_scene->setSceneRect(0, 0, m_width, m_height);
    this->addGrid();

    QImage image(m_width, m_height, QImage::Format_ARGB32);
    image.fill(Qt::white);
    m_pixmapItem = m_scene->addPixmap(QPixmap::fromImage(image));

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
    //изображение с пикселями
    //2k x 2k = 130мс, без генерации цвета на каждом шаге = 40мс
    QImage image = m_pixmapItem->pixmap().toImage();
    QRgb* data = reinterpret_cast<QRgb*>(image.bits());
    for (qsizetype y = 0; y < m_height; ++y)
        for (qsizetype x = 0; x < m_width; ++x)
        {
            data[y * m_width + x] = qRgb(
                        QRandomGenerator::global()->bounded(256),
                        QRandomGenerator::global()->bounded(256),
                        QRandomGenerator::global()->bounded(256));
        }
    m_pixmapItem->setPixmap(QPixmap::fromImage(image));

    //выводим информацию о номере шага, время итерации
    m_ui->stepNumber->setNum(m_ui->stepNumber->text().toInt() + 1);
    m_ui->time->setNum(QTime::currentTime().msecsSinceStartOfDay() - m_ms);
    m_ms = QTime::currentTime().msecsSinceStartOfDay();
}

void MainWindow::addGrid()
{
    QPainterPath gridPath;
    for (qsizetype x = 0; x <= m_width; ++x)
    {
        gridPath.moveTo(x, 0);
        gridPath.lineTo(x, m_height);

    }
    for (qsizetype y = 0; y <= m_height; ++y)
    {
        gridPath.moveTo(0, y);
        gridPath.lineTo(m_width, y);
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
}
