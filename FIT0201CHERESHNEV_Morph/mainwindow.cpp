#include <QFileDialog>
#include <QPainter>
#include <QRectF>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "translator.h"

static const QString TEXTURE_PATH = "://morph_src.png";
static const int INIT_SIDE_LENGTH = 256;
static const QPoint C_INIT(0, INIT_SIDE_LENGTH / 2);
static const QPoint F_INIT(INIT_SIDE_LENGTH - 1, INIT_SIDE_LENGTH / 2);
static const int POINT_POSITION_NUMBER = 100;

static const QRectF TOP_MEDIATE_RECT(0., 0., 1., .5);
static const QRectF BOTTOM_MEDIATE_RECT(0., .5, 1., .5);

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	buffer(INIT_SIDE_LENGTH, INIT_SIDE_LENGTH, QImage::Format_ARGB32),
	mipMap(TEXTURE_PATH, Point, None),
	sideLength(INIT_SIDE_LENGTH),
	cPoint(C_INIT),
	fPoint(F_INIT),
	topQuadMapper(topQuadrangle(), TOP_MEDIATE_RECT),
	bottomQuadMapper(bottomQuadrangle(), BOTTOM_MEDIATE_RECT)
{
	ui->setupUi(this);
	updateBuffer();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::updateBuffer()
{
	buffer.fill(Qt::white);
	topQuadMapper.draw(mipMap, buffer);
	bottomQuadMapper.draw(mipMap, buffer);

	//drawing buffer on pixmap in the center so offsets is calculated
	QRect bRect = boundingRect();
	int leftOffset = qMax(0, -bRect.left());
	int rightOffset = qMax(0, bRect.right() - sideLength);

	QPixmap pixmap(2 * qMax(leftOffset, rightOffset) + sideLength, sideLength);
	pixmap.fill(Qt::white);

	int offset = rightOffset - leftOffset;
	QPainter painter;
	painter.begin(&pixmap);
	painter.drawImage(offset > 0 ? offset : 0, 0, buffer);
	painter.end();

	emit pointCChanged(cPoint.x());
	emit pointFChanged(fPoint.x());
	ui->imageLabel->setPixmap(pixmap);
}

void MainWindow::resizeBuffer()
{
	buffer = QImage(boundingRect().size(), QImage::Format_ARGB32);
}

QRect MainWindow::boundingRect()
{
	int xMin = qMin(cPoint.x(), fPoint.x());
	int xMax = qMax(cPoint.x(), fPoint.x());
	xMin = qMin(xMin, 0);
	xMax = qMax(xMax, sideLength - 1);
	return QRect(xMin, 0, xMax - xMin + 1, sideLength);
}

int MainWindow::mostLeftX()
{
	return -sideLength / 2;
}

int MainWindow::mostRightX()
{
	return 3 * sideLength / 2;
}

Utils::Quadrangle MainWindow::topQuadrangle()
{
	QPoint off(boundingRect().topLeft());
	return Utils::Quadrangle(-off,
							 QPoint(sideLength - 1, 0) - off,
							 cPoint - off,
							 fPoint - off);
}

Utils::Quadrangle MainWindow::bottomQuadrangle()
{
	QPoint off(boundingRect().topLeft());
	return Utils::Quadrangle(cPoint - off,
							 fPoint - off,
							 QPoint(0, sideLength - 1) - off,
							 QPoint(sideLength - 1, sideLength - 1) - off);
}

void MainWindow::createQuadrangles()
{
	topQuadMapper = QuadrangleMapper(topQuadrangle(), TOP_MEDIATE_RECT);
	bottomQuadMapper = QuadrangleMapper(bottomQuadrangle(), BOTTOM_MEDIATE_RECT);
}

void MainWindow::movePointC(int pos)
{
	cPoint.setX(qRound(mostLeftX() + 1. * pos * (mostRightX() - mostLeftX()) / POINT_POSITION_NUMBER));
	createQuadrangles();
	resizeBuffer();
	updateBuffer();
}

void MainWindow::movePointF(int pos)
{
	fPoint.setX(qRound(mostLeftX() + 1. * pos * (mostRightX() - mostLeftX()) / POINT_POSITION_NUMBER));
	createQuadrangles();
	resizeBuffer();
	updateBuffer();
}

void MainWindow::setPointLayerFiltering()
{
	mipMap.setLayerFiltering(Point);
	updateBuffer();
}

void MainWindow::setLinearLayerFiltering()
{
	mipMap.setLayerFiltering(Linear);
	updateBuffer();
}

void MainWindow::setNoneMipFiltering()
{
	mipMap.setMipFiltering(None);
	updateBuffer();
}

void MainWindow::setPointMipFiltering()
{
	mipMap.setMipFiltering(Point);
	updateBuffer();
}

void MainWindow::setLinearMipFiltering()
{
	mipMap.setMipFiltering(Linear);
	updateBuffer();
}

void MainWindow::setWidth(int newWidth)
{
	qreal ratio = 1. * newWidth / sideLength;
	cPoint *= ratio;
	fPoint *= ratio;
	sideLength = newWidth;
	createQuadrangles();
	resizeBuffer();
	updateBuffer();
}

void MainWindow::openTexture()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Load texture", ".", "Images (*.png *.bmp *.jpg)");
	if (!fileName.isEmpty())
	{
		mipMap = MipMap(fileName, mipMap.getLayerFiltering(), mipMap.getMipFiltering());
		updateBuffer();
	}
}
