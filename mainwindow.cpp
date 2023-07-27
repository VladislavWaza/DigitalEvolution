#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "region.h"
#include "graphicsviewzoom.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    GraphicsViewZoom* zoom = new GraphicsViewZoom(ui->graphicsView);
    zoom->setModifiers(Qt::NoModifier);

    _scene = new PaintableScene(this);
    ui->graphicsView->setScene(_scene);
    connect(_scene, &PaintableScene::signalPainting, this, &MainWindow::slotPainting);

    ui->heightWorld->setMaximum(1000);
    ui->widthWorld->setMaximum(1000);
    ui->heightWorld->setMinimum(50);
    ui->widthWorld->setMinimum(50);

    ui->brushDiameter->setMinimum(1);
    ui->brushDiameter->setMaximum(500);
    ui->brushDiameter->setEnabled(false);
    ui->label_3->setEnabled(false);
}

MainWindow::~MainWindow()
{

    disconnect(_scene, &PaintableScene::signalPainting, this, &MainWindow::slotPainting);
    delete ui;
    delete _scene;
}


void MainWindow::on_createWorld_clicked()
{
    _regions.clear();
    _scene->clear();

    int heightWorld = ui->heightWorld->value();
    int widthWorld = ui->widthWorld->value();
    _scene->setSceneRect(0, 0, heightWorld * 5, widthWorld * 5);
    _scene->setBackgroundBrush(Qt::blue);

    ui->brushDiameter->setEnabled(true);
    ui->label_3->setEnabled(true);

    QBrush brush(Qt::gray);
    QPen pen;
    pen.setWidthF(0.25);
    Region* item;

    for (int i = 0; i < widthWorld; ++i)
    {
        for (int j = 0; j < heightWorld; ++j)
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
            //очень не безопасно, если item не наследник QAbstractGraphicsShapeItem
            QAbstractGraphicsShapeItem *rect = static_cast<QAbstractGraphicsShapeItem*>(item);
            rect->setBrush(Qt::white);
        }
    }
}

