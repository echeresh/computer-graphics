#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QApplication>
#include <QtGui>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<QImage>("QImage"); //for passing QImage between threads
    MainWindow w;
    w.show();

   return a.exec();
}
