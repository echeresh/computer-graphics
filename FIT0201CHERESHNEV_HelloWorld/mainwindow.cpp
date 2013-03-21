#include <QDebug>
#include <QPainter>
#include <QtCore/qmath.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    areaImage(MAX_AREA_WIDTH, MAX_AREA_HEIGHT, QImage::Format_RGB888)
{
    ui->setupUi(this);
    clearArea();
    update();
    setMaximumHeight(MAX_AREA_HEIGHT);
    setMaximumWidth(MAX_AREA_WIDTH);
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
    for (int x = 0; x < MAX_AREA_WIDTH; x++)
    {
        int y = static_cast<int>(MAX_AREA_HEIGHT / 2 - UNIT_LENGTH * qSin(static_cast<qreal>(x - MAX_AREA_WIDTH / 2) / UNIT_LENGTH) + .5);
        areaImage.setPixel(x, y, QColor(Qt::red).rgba());
    }
    update();
}

void MainWindow::on_drawAxisButton_clicked()
{
    //drawing y-Axis
    for (int x = MAX_AREA_WIDTH / 2, y = 0; y < MAX_AREA_HEIGHT; y++)
    {
        areaImage.setPixel(x, y, QColor(Qt::black).rgba());
    }
    for (int y = MAX_AREA_HEIGHT / 2, nUnit = 0; y < MAX_AREA_HEIGHT; y += UNIT_LENGTH, nUnit++)
    {
        for (int x = MAX_AREA_WIDTH / 2 - MARK_HALF_LENGTH; x <= MAX_AREA_WIDTH / 2 + MARK_HALF_LENGTH; x++)
        {
            areaImage.setPixel(x, y, QColor(Qt::black).rgba());
            areaImage.setPixel(x, y - 2 * nUnit * UNIT_LENGTH, QColor(Qt::black).rgba());
        }
    }
    //drawing x-Axis
    for (int y = MAX_AREA_HEIGHT / 2, x = 0; x < MAX_AREA_WIDTH; x++)
    {
        areaImage.setPixel(x, y, QColor(Qt::black).rgba());
    }
    for (int x = MAX_AREA_WIDTH / 2, nUnit = 0; x < MAX_AREA_WIDTH; x += UNIT_LENGTH, nUnit++)
    {
        for (int y = MAX_AREA_HEIGHT / 2 - MARK_HALF_LENGTH; y <= MAX_AREA_HEIGHT / 2 + MARK_HALF_LENGTH; y++)
        {
            areaImage.setPixel(x, y, QColor(Qt::black).rgba());
            areaImage.setPixel(x - 2 * nUnit * UNIT_LENGTH, y, QColor(Qt::black).rgba());
        }
    }
    update();
}

void MainWindow::on_clearButton_clicked()
{
    clearArea();
    update();
}
