#include <QGraphicsPathItem>
#include <QPainterPath>
#include "mainwindow.h"


MainWindow::SelectionUI::SelectionUI()
{
    _clan = nullptr;
    _region = nullptr;
    _selectionItem = nullptr;
}

void MainWindow::SelectionUI::clear()
{
    _clan = nullptr;
    _region = nullptr;
    _selectionItem = nullptr;
    //очищение ui
}

Region *MainWindow::SelectionUI::region()
{
    return _region;
}

void MainWindow::SelectionUI::select(int x, int y, World *world)
{
    _clan = world->getClan(x,y);
    _region = world->getRegion(x,y);
}

void MainWindow::SelectionUI::displayInfo(Ui::MainWindow *ui)
{

}

void MainWindow::SelectionUI::updateClanPos(World *world, PaintableScene *scene)
{
    QPoint pos = world->getClanPos(_clan);
    if (pos != World::ClanUndefined) //Если _clan еще существует
    {
        if (_selectionItem)
            scene->removeItem(_selectionItem);
        QPainterPath path;
        path.addRect(pos.x(),pos.y(),1,1);
        _selectionItem = scene->addPath(path, QPen(Qt::white, 0.1));
        _selectionItem->setZValue(static_cast<int>(MainWindow::LayerLevel::HighlightSelection));
    }
    else
        _clan = nullptr;
}

void MainWindow::SelectionUI::displaySelection(int x, int y, PaintableScene *scene)
{
    if (_selectionItem)
    {
        scene->removeItem(_selectionItem);
        _selectionItem = nullptr;
    }
    if (_clan || _region)
    {
        QPainterPath path;
        path.addRect(x,y,1,1);
        _selectionItem = scene->addPath(path, QPen(Qt::white, 0.1));
        _selectionItem->setZValue(static_cast<int>(MainWindow::LayerLevel::HighlightSelection));
    }
}
