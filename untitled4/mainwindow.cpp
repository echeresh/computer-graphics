#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QRect>


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

}
void MainWindow::paintEvent(QPaintEvent *)
{
	QRect rect(0, 0, 10, 10);
	bool b = rect.contains(9, 10);

	QImage img;
	img.load(QString("C://m.png"));
	//img.fill(QColor(255, 255, 255, 255));
	QPainter painter(&img);
	painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
	QPen pen(Qt::DashLine);
	pen.setColor(QColor(100, 100, 100, 255));
	painter.setPen(pen);
	painter.drawLine(100, 100, 400, 400);

	//painter.setPen(QColor(100, 100, 100, 255));
	//painter.drawLine(100, 100, 400, 400);
	QPainter painter1(this);
	painter1.drawImage(0, 0, img);
	//painter.drawLine(100, 100, 200, 200);
}
MainWindow::~MainWindow()
{
	delete ui;
}
