#include <QDebug>
#include <QPainter>
#include <QtCore/qmath.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

//const int MainWindow::maxAreaWidth;
//const int MainWindow::maxAreaHeight;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    unitLength(20),
    maxAreaWidth(40*unitLength),
    maxAreaHeight(15*unitLength),
    areaImage(maxAreaWidth, maxAreaHeight, QImage::Format_RGB888)
{
    ui->setupUi(this);
    clearArea();
    setMaximumHeight(maxAreaHeight);
    setMaximumWidth(maxAreaWidth);
    connect(ui->drawSinButton, SIGNAL(clicked()), SLOT(drawSinus()));
    connect(ui->drawAxisButton, SIGNAL(clicked()), SLOT(drawAxis()));
    connect(ui->clearButton, SIGNAL(clicked()), SLOT(clearArea()));
}

void MainWindow::drawAxis()
{
    const int markLength = 3;
    //drawing y-Axis
    for (int x = maxAreaWidth/2, y = 0; y < maxAreaHeight; y++)
    {
        areaImage.setPixel(x, y, QColor(0, 0, 0).rgba());
    }
    for (int y = maxAreaHeight/2, nUnit = 0; y < maxAreaHeight; y += unitLength, nUnit++)
    {
        for (int x = maxAreaWidth/2 - markLength; x <= maxAreaWidth/2 + markLength; x++)
        {
            areaImage.setPixel(x, y, QColor(0, 0, 0).rgba());
            areaImage.setPixel(x, y - 2 * nUnit * unitLength, QColor(0, 0, 0).rgba());
        }
    }
    //drawing x-Axis
    for (int y = maxAreaHeight/2, x = 0; x < maxAreaWidth; x++)
    {
        areaImage.setPixel(x, y, QColor(0, 0, 0).rgba());
    }
    for (int x = maxAreaWidth/2, nUnit = 0; x < maxAreaWidth; x += unitLength, nUnit++)
    {
        for (int y = maxAreaHeight/2 - markLength; y <= maxAreaHeight/2 + markLength; y++)
        {
            areaImage.setPixel(x, y, QColor(0, 0, 0).rgba());
            areaImage.setPixel(x - 2*nUnit*unitLength, y, QColor(0, 0, 0).rgba());
        }
    }
    update();
}

void MainWindow::drawSinus()
{
    for (int x = 0; x < maxAreaWidth; x++)
    {
        int y = static_cast<int>(maxAreaHeight/2 + unitLength*qSin(static_cast<qreal>(x - maxAreaWidth/2)/unitLength) + .5);
        areaImage.setPixel(x, y, QColor(0, 0, 0).rgba()); update();
    }
    update();
}

void MainWindow::clearArea()
{
     areaImage.fill(Qt::white);
     update();
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(0, 0, areaImage);
}

MainWindow::~MainWindow()
{
    delete ui;
}
