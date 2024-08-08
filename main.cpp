#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    try {
        MainWindow w;
        w.show();
        return a.exec();
    } catch (const std::exception &e) {
        QMessageBox::critical(nullptr, "Error", e.what());
        return EXIT_FAILURE;
    } catch (...) {
        QMessageBox::critical(nullptr, "Error", "An unknown error occurred.");
        return EXIT_FAILURE;
    }
}
