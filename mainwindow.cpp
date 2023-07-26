#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "region.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene = new PaintableScene(this);
    ui->graphicsView->setScene(scene);
    connect(scene, &PaintableScene::signalPainting, this, &MainWindow::slotPainting);

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

    disconnect(scene, &PaintableScene::signalPainting, this, &MainWindow::slotPainting);
    delete ui;
    delete scene;
}


void MainWindow::on_createWorld_clicked()
{
    regions.clear();
    scene->clear();

    int height = ui->heightWorld->value();
    int width = ui->widthWorld->value();
    scene->setSceneRect(0, 0, height * 5, width * 5);
    scene->setBackgroundBrush(Qt::blue);

    ui->brushDiameter->setEnabled(true);
    ui->label_3->setEnabled(true);

    QBrush brush(Qt::gray);
    QPen pen;
    pen.setWidthF(0.5);
    Region* item;

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            item = new Region(i*5, j*5, 5, 5);
            item->setBrush(brush);
            item->setPen(pen);
            regions.append(item);
            scene->addItem(item);
        }
    }
}

void MainWindow::slotPainting(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (ui->brushDiameter->isEnabled())
    {
        QPainterPath circle;
        circle.addEllipse(mouseEvent->scenePos(), ui->brushDiameter->value(), ui->brushDiameter->value());
        QList<QGraphicsItem*> items = scene->items(circle, Qt::IntersectsItemShape, Qt::DescendingOrder);
        for (QGraphicsItem* item : items)
        {
            //очень не безопасно, если item не наследник QAbstractGraphicsShapeItem
            QAbstractGraphicsShapeItem *rect = reinterpret_cast<QAbstractGraphicsShapeItem*>(item);
            rect->setBrush(Qt::white);
        }
    }
}

