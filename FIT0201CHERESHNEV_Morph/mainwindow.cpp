#include <QDebug>
#include <QMouseEvent>
#include <QFileDialog>
#include <QPainter>
#include <QRectF>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "translator.h"

static const QString TEXTURE_PATH = "://morph_src.png";

static const int INIT_SIDE_LENGTH = 256;
static const QPoint F_INIT(INIT_SIDE_LENGTH - 1, INIT_SIDE_LENGTH / 2);
static const QPoint C_INIT(0, INIT_SIDE_LENGTH / 2);

static const int POINT_POSITION_NUMBER = 100;

static const QRectF TOP_MEDIATE_RECT(0., 0., 1., .5);
static const QRectF BOTTOM_MEDIATE_RECT(0., .5, 1., .5);

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	mipMap(TEXTURE_PATH, Point, None),
	sideLength(INIT_SIDE_LENGTH),
	cPoint(C_INIT),
	fPoint(F_INIT)
{
	ui->setupUi(this);
	createQuadrangles();
	resizeBuffer();
	updateBuffer();
}

MainWindow::~MainWindow()
{
	delete ui;
}

QPair<int, int> MainWindow::getOffsets()
{
	QRect bRect = boundingRect();
	int leftOffset = qMax(0, -bRect.left());
	int rightOffset = qMax(0, bRect.right() - sideLength);
	return qMakePair(qMax(leftOffset, rightOffset), qMax(rightOffset - leftOffset, 0));
}

void MainWindow::updateBuffer()
{
	buffer.fill(Qt::white);
	topQuadMapper.draw(mipMap, buffer);
	bottomQuadMapper.draw(mipMap, buffer);

	QPair<int, int> offsets = getOffsets();
	QPixmap pixmap(2 * offsets.first + sideLength, sideLength);
	pixmap.fill(Qt::white);

	QPainter painter;
	painter.begin(&pixmap);
	painter.drawImage(offsets.second, 0, buffer);
	painter.end();

	emit pointCChanged(cPoint.x());
	emit pointFChanged(fPoint.x());
	ui->imageLabel->setPixmap(pixmap);
}

void MainWindow::resizeBuffer()
{
	buffer = QImage(boundingRect().size(), QImage::Format_RGB32);
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
	do
	{
		topQuadMapper = QuadrangleMapper(topQuadrangle(), TOP_MEDIATE_RECT);
		bottomQuadMapper = QuadrangleMapper(bottomQuadrangle(), BOTTOM_MEDIATE_RECT);
		cPoint.rx()++;
	}
	while(!topQuadMapper.isValid() || !bottomQuadMapper.isValid());
	cPoint.rx()--;
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

void MainWindow::mousePressEvent(QMouseEvent *e)
{
	QPair<int, int> offsets = getOffsets();
	QPoint imageStart((ui->imageLabel->width() - 2 * offsets.first - sideLength) / 2 + offsets.second,
					  (ui->imageLabel->height() - sideLength) / 2);
	imageStart += ui->imageLabel->pos();
	QPoint imagePoint(e->pos() - imageStart);

	int x = imagePoint.x();
	int y = imagePoint.y();
	QuadrangleMapper& mapper = y < sideLength / 2 ? topQuadMapper : bottomQuadMapper;
	Utils::QuadrangleF quad =
	{
		mapper.translate(QPointF(x + 0., y + 0.)),
		mapper.translate(QPointF(x + 0., y + 1.)),
		mapper.translate(QPointF(x + 1., y + 0.)),
		mapper.translate(QPointF(x + 1., y + 1.)),
	};
	qDebug()<<"clicked: "<<imagePoint;
	qDebug()<<"uv: "<<quad.p0<<" "<<quad.p1<<" "<<quad.p2<<" "<<quad.p3<<endl;
}

void MainWindow::setDefaultViewMode()
{
	mipMap.setViewMode(Default);
	updateBuffer();
}

void MainWindow::setUVViewMode()
{
	mipMap.setViewMode(UV);
	updateBuffer();
}

void MainWindow::setMIPViewMode()
{
	mipMap.setViewMode(MIP);
	updateBuffer();
}

void MainWindow::setMaxMipLevel(int maxLevel)
{
	mipMap.setMaxLevel(maxLevel);
	updateBuffer();
}
