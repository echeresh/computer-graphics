#include <QDebug>
#include <QPainter>
#include <QtCore/qmath.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    update();
    areaImage.setPixel(areaImage.width() - 1, areaImage.height() - 1, QColor(Qt::red).rgba());
    setMaximumHeight(maxAreaHeight);
    setMaximumWidth(maxAreaWidth);
    connect(ui->drawSinButton, SIGNAL(clicked()), SLOT(drawSinus()));
    connect(ui->drawAxisButton, SIGNAL(clicked()), SLOT(drawAxis()));
    connect(ui->clearButton, SIGNAL(clicked()), SLOT(clearArea()));
}

void MainWindow::clearArea()
{
    areaImage.fill(Qt::white);
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

void MainWindow::on_drawSinButton_clicked()
{
    for (int x = 0; x < maxAreaWidth; x++)
    {
        int y = static_cast<int>(maxAreaHeight/2 + unitLength*qSin(static_cast<qreal>(x - maxAreaWidth/2)/unitLength) + .5);
        areaImage.setPixel(x, y, QColor(Qt::black).rgba());
    }
    update();
}

void MainWindow::on_drawAxisButton_clicked()
{
    const int markLength = 3;
    //drawing y-Axis
    for (int x = maxAreaWidth/2, y = 0; y < maxAreaHeight; y++)
    {
        areaImage.setPixel(x, y, QColor(Qt::black).rgba());
    }
    for (int y = maxAreaHeight/2, nUnit = 0; y < maxAreaHeight; y += unitLength, nUnit++)
    {
        for (int x = maxAreaWidth/2 - markLength; x <= maxAreaWidth/2 + markLength; x++)
        {
            areaImage.setPixel(x, y, QColor(Qt::black).rgba());
            areaImage.setPixel(x, y - 2 * nUnit * unitLength, QColor(Qt::black).rgba());
        }
    }
    //drawing x-Axis
    for (int y = maxAreaHeight/2, x = 0; x < maxAreaWidth; x++)
    {
        areaImage.setPixel(x, y, QColor(Qt::black).rgba());
    }
    for (int x = maxAreaWidth/2, nUnit = 0; x < maxAreaWidth; x += unitLength, nUnit++)
    {
        for (int y = maxAreaHeight/2 - markLength; y <= maxAreaHeight/2 + markLength; y++)
        {
            areaImage.setPixel(x, y, QColor(Qt::black).rgba());
            areaImage.setPixel(x - 2*nUnit*unitLength, y, QColor(Qt::black).rgba());
        }
    }
    update();
}

void MainWindow::on_clearButton_clicked()
{
    clearArea();
    update();
}
