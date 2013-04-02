#include <QDebug>
#include <qmath.h>
#include <QPainter>
#include <QMatrix4x4>
#include <QVector3D>
#include <QFileDialog>
#include <QVector>
#include <QPair>
#include <QDebug>
#include "utils.h"
#include "widget.h"

static const float RMIN = 0.05f;
static const float RMAX = 0.95f;
static const float RSTEP = 0.025f;
static const float RINIT = 0.5f;
static const QString MODEL_FILE("://teapotCGA.bpt");

static const QRgb XAXIS_COLOR = qRgb(255, 0, 0);
static const QRgb YAXIS_COLOR = qRgb(0, 255, 0);
static const QRgb ZAXIS_COLOR = qRgb(0, 0, 255);
static const QRgb BOX_COLOR = qRgb(0xC0, 0xC0, 0xC0);
static const QRgb MODEL_COLOR = qRgb(0, 0, 0);

static const int INIT_SCALE = 200.;
static const int INIT_SEGMENTS = 8;
static const QVector3D INIT_EYE(0, -10, 0);
static const QVector3D INIT_UP(0, 0, 1);

Widget::Widget(QWidget *parent) :
	QWidget(parent),
	eye(INIT_EYE),
	up(INIT_UP),
	R(RINIT),
	scale(INIT_SCALE),
	bezierSurface(MODEL_FILE)
{
	leftPressed = false;
	calcGrid(INIT_SEGMENTS);
}

void Widget::mouseMoveEvent(QMouseEvent *mouseEvent)
{
	if (leftPressed)
	{
		QPoint dp = mouseEvent->pos() - lastPoint;
		lastPoint = mouseEvent->pos();
		QMatrix4x4 xMatrix;
		xMatrix.rotate(dp.x(), up);
		eye = xMatrix.map(eye);

		QMatrix4x4 yMatrix;
		yMatrix.rotate(-dp.y(), QVector3D::crossProduct(eye, up));
		eye = yMatrix.map(eye);
		up = yMatrix.map(up);
		updatePlot();
	}
}

void Widget::mousePressEvent(QMouseEvent *mouseEvent)
{
	if (mouseEvent->button() == Qt::LeftButton)
	{
		leftPressed = true;
		lastPoint = mouseEvent->pos();
	}
}

void Widget::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
	if (mouseEvent->button() == Qt::LeftButton)
	{
		leftPressed = false;
	}
}

void Widget::wheelEvent(QWheelEvent *wheelEvent)
{
	int dR = wheelEvent->angleDelta().y() / 120;
	float newR = R - dR * RSTEP;
	newR = qMax(newR, RMIN);
	R = qMin(newR, RMAX);
	updatePlot();
}

void Widget::resizeEvent(QResizeEvent *)
{
	resizeBuffer(size());
	updatePlot();
}

QPoint Widget::to2D(const QVector3D& v)
{
	QVector3D right = -QVector3D::crossProduct(eye, up);
	right.normalize();
	int x = static_cast<int>(scale * QVector3D::dotProduct(v, right) + .5);
	int y = static_cast<int>(scale * QVector3D::dotProduct(v, up) + .5);
	return QPoint(buffer.width() / 2 + x, buffer.height() / 2 - y);
}

QPoint Widget::getProjection(const QVector3D& v)
{
	QVector3D camera(-eye);
	QVector3D planePoint(R * camera);
	QVector3D dv(v - camera);
	qreal d = QVector3D::dotProduct(planePoint - camera, eye) /
			QVector3D::dotProduct(dv, eye);
	QVector3D p = camera + d * dv;
	if (Utils::sign((v - p).x() / dv.x()) == Utils::sign(-d))
	{
		return QPoint();
	}
	return to2D(p);
}

QPoint Widget::getIntersection(const QVector3D& v0, const QVector3D& v1)
{
	QVector3D camera(-eye);
	QVector3D dv(v0 - v1);
	qreal d = QVector3D::dotProduct(R * camera - v1, eye) /	QVector3D::dotProduct(dv, eye);
	return to2D(v1 + d * dv);
}

QVector3D Widget::calcBoundingBox()
{
	if (grids.isEmpty())
	{
		lines.clear();
		return QVector3D();
	}
	QVector3D vMin(FLT_MAX, FLT_MAX, FLT_MAX);
	QVector3D vMax(FLT_MIN, FLT_MIN, FLT_MIN);
	for (int i = 0; i < grids.count(); i++)
	{
		vMax = Utils::max(grids[i].getMaxPoint(), vMax);
		vMin = Utils::min(grids[i].getMinPoint(), vMin);
	}
	QVector3D center = (vMin + vMax) / 2;
	for (int i = 0; i < grids.count(); i++)
	{
		grids[i].center(center);
	}
	lines = createBox(vMax - center);
	return vMax - center;
}

void Widget::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.drawImage(0, 0, buffer);
}

void Widget::drawAxis(const QPoint &center, const QPoint &dirVector, QRgb rgb)
{
	if (center == dirVector)
	{
		return;
	}
	setColor(rgb);
	QPoint dp(dirVector - center);
	QPoint to(dirVector);
	while(buffer.rect().contains(to))
	{
		to += dp;
	}
	drawLine(center, to);
}

void Widget::updatePlot()
{
	buffer.fill(Qt::white);
	QPoint center(to2D(QVector3D()));
	drawAxis(center, to2D(QVector3D(100, 0, 0)), XAXIS_COLOR);
	drawAxis(center, to2D(QVector3D(0, 100, 0)), YAXIS_COLOR);
	drawAxis(center, to2D(QVector3D(0, 0, 100)), ZAXIS_COLOR);
	setColor(BOX_COLOR);
	for (int i = 0; i < lines.count(); i++)
	{
		drawLine(lines[i].first, lines[i].second);
	}

	setColor(MODEL_COLOR);
	for (int i = 0; i < grids.count(); i++)
	{
		grids[i].draw(*this);
	}

	update();
}

QVector<Utils::Line3D> Widget::createBox(const QVector3D& v)
{
	QVector<Utils::Line3D> edges;
	edges.append(Utils::makeLine3D(QVector3D(v.x(), v.y(), v.z()), QVector3D(v.x(), v.y(), -v.z())));
	edges.append(Utils::makeLine3D(QVector3D(v.x(), v.y(), v.z()), QVector3D(v.x(), -v.y(), v.z())));
	edges.append(Utils::makeLine3D(QVector3D(v.x(), v.y(), v.z()), QVector3D(-v.x(), v.y(), v.z())));

	edges.append(Utils::makeLine3D(QVector3D(-v.x(), -v.y(), -v.z()), QVector3D(v.x(), -v.y(), -v.z())));
	edges.append(Utils::makeLine3D(QVector3D(-v.x(), -v.y(), -v.z()), QVector3D(-v.x(), v.y(), -v.z())));
	edges.append(Utils::makeLine3D(QVector3D(-v.x(), -v.y(), -v.z()), QVector3D(-v.x(), -v.y(), v.z())));

	edges.append(Utils::makeLine3D(QVector3D(-v.x(), -v.y(), v.z()), QVector3D(v.x(), -v.y(), v.z())));
	edges.append(Utils::makeLine3D(QVector3D(-v.x(), -v.y(), v.z()), QVector3D(-v.x(), v.y(), v.z())));

	edges.append(Utils::makeLine3D(QVector3D(v.x(), -v.y(), -v.z()), QVector3D(v.x(), -v.y(), v.z())));
	edges.append(Utils::makeLine3D(QVector3D(v.x(), -v.y(), -v.z()), QVector3D(v.x(), v.y(), -v.z())));

	edges.append(Utils::makeLine3D(QVector3D(-v.x(), v.y(), -v.z()), QVector3D(v.x(), v.y(), -v.z())));
	edges.append(Utils::makeLine3D(QVector3D(-v.x(), v.y(), -v.z()), QVector3D(-v.x(), v.y(), v.z())));
	return edges;
}

void Widget::calcGrid(int nSegments)
{
	this->nSegments = nSegments;
	grids = bezierSurface.getGrids(nSegments);
	aabb = calcBoundingBox();
	emit segmentsChanged(nSegments);
}

void Widget::updateSegments(int nSegments)
{
	calcGrid(nSegments);
	updatePlot();
}

void Widget::openFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Open Model file", ".", "*.bpt");
	qDebug()<<fileName;
	if (!fileName.isEmpty())
	{
		bezierSurface = BezierSurface(fileName);
		grids = bezierSurface.getGrids(nSegments);
		aabb = calcBoundingBox();
		updatePlot();
	}
}
