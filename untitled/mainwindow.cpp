#include <QImage>
#include <QGraphicsScene>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QImage image("://new//prefix1//1.jpg");
    item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    scene = new QGraphicsScene;
    scene->addItem(item);
    ui->graphicsView->setScene(scene);
    item->setPixmap(QPixmap::fromImage(image));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete scene;
}
