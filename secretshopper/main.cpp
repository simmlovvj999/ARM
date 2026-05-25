#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QDebug>

int main(int argc, char *argv[]) {
    qDebug() << "main start";
    QApplication a(argc, argv);
    qDebug() << "QApplication created";

    a.setStyle(QStyleFactory::create("Fusion"));
    qDebug() << "style set";

    MainWindow w;
    qDebug() << "MainWindow created";

    w.showMaximized();
    qDebug() << "shown";

    return a.exec();
}
