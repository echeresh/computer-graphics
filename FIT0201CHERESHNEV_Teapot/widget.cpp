#include <qmath.h>
#include <QPainter>
#include <QVector3D>
#include <QFileDialog>
#include <QVector>
#include "utils.h"
#include "widget.h"

static const QString MODEL_FILE("://teapotCGA.bpt");

static const QRgb XAXIS_COLOR = qRgb(0xFF, 0, 0);
static const QRgb YAXIS_COLOR = qRgb(0, 0xFF, 0);
static const QRgb ZAXIS_COLOR = qRgb(0, 0, 0xFF);
static const QRgb BOX_COLOR = qRgb(0xC0, 0xC0, 0xC0);
static const QRgb FILL_COLOR = qRgb(0x50, 0x50, 0xFF);
static const QRgb FRAME_COLOR = qRgb(0, 0, 0);
static const int INIT_SEGMENTS = 5;

Widget::Widget(QWidget *parent) :
	QWidget(parent)
{
	showAxis = true;
	showBoundingBox = true;
	showLight = false;
	showPolygons = false;
	showFilling = false;
	showFrame = true;

	nSegments = INIT_SEGMENTS;

	leftPressed = false;
	loadBezierSurface(MODEL_FILE);
}

void Widget::mouseMoveEvent(QMouseEvent *mouseEvent)
{
	if (leftPressed)
	{
		QPoint dp = mouseEvent->pos() - lastPoint;
		lastPoint = mouseEvent->pos();
		xRotate(dp.x());
		yRotate(dp.y());
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
	const int DELTA_UNIT = 120;
	zoom(wheelEvent->angleDelta().y() / DELTA_UNIT);
	updatePlot();
}

void Widget::resizeEvent(QResizeEvent *)
{
	resizeBuffer(size());
	updatePlot();
}

void Widget::calcBoundingBox()
{
	if (!bezierModel.isEmpty())
	{
		boundingBox =  bezierModel.center();
	}
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

void Widget::loadBezierSurface(const QString& filePath)
{
	bezierModel = BezierSurfaceModel(filePath);
	calcBoundingBox();
	grids = bezierModel.getGrids(nSegments);
	recalcView(boundingBox);
}

void Widget::updatePlot()
{
	buffer.fill(Qt::white);
	if (showAxis)
	{
		QPoint center(getProjection(QVector3D()));
		float AXIS_UNIT = boundingBox.length();
		drawAxis(center, getProjection(QVector3D(AXIS_UNIT, 0, 0)), XAXIS_COLOR);
		drawAxis(center, getProjection(QVector3D(0, AXIS_UNIT, 0)), YAXIS_COLOR);
		drawAxis(center, getProjection(QVector3D(0, 0, AXIS_UNIT)), ZAXIS_COLOR);
	}
	if (showBoundingBox)
	{
		setColor(BOX_COLOR);
		drawBox(boundingBox);
	}
	if (showPolygons)
	{
		QVector<Utils::Triangle3D> triangles;
		for (int i = 0; i < grids.count(); i++)
		{
			triangles += grids[i].getTriangles(*this);
		}
		drawTriangles(triangles, showFrame, FRAME_COLOR, showFilling, FILL_COLOR, showLight);
	}
	else if (showFrame)
	{
		setColor(FRAME_COLOR);
		bezierModel.draw(nSegments, *this);
	}
	setColor(qRgb(0, 0, 0));
	drawLine(QPoint(0, 0), QPoint(buffer.width() - 1, 0));
	drawLine(QPoint(0, 0), QPoint(0, buffer.height() - 1));
	drawLine(QPoint(buffer.width() - 1, 0), QPoint(buffer.width() - 1, buffer.height() - 1));
	drawLine(QPoint(0, buffer.height() - 1), QPoint(buffer.width() - 1, buffer.height() - 1));
	update();
}

void Widget::openModelFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Open model file", ".", "*.bpt");
	if (!fileName.isEmpty())
	{
		loadBezierSurface(fileName);
		updatePlot();
	}
}

void Widget::updateSegments(int nSegments)
{
	this->nSegments = nSegments;
	grids = bezierModel.getGrids(nSegments);
	emit segmentsChanged(nSegments);
	updatePlot();
}

void Widget::updateFilling(bool showFilling)
{
	this->showFilling = showFilling;
	updatePlot();
}

void Widget::updateLight(bool showLight)
{
	this->showLight = showLight;
	updatePlot();
}

void Widget::updateBoundingBox(bool showBoundingBox)
{
	this->showBoundingBox = showBoundingBox;
	updatePlot();
}

void Widget::updateAxis(bool showAxis)
{
	this->showAxis = showAxis;
	updatePlot();
}

void Widget::setBezierMode()
{
	this->showPolygons = false;
	updatePlot();
}

void Widget::setPolygonMode()
{
	this->showPolygons = true;
	updatePlot();
}

void Widget::updateFrame(bool showFrame)
{
	this->showFrame = showFrame;
	updatePlot();
}

void Widget::setStepByStepBezierDrawing()
{
	setBezierDrawing(STEP_BY_STEP_BEZIER_DRAWING);
	updatePlot();
}

void Widget::setIterativeBezierDrawing()
{
	setBezierDrawing(ITERATIVE_BEZIER_DRAWING);
	updatePlot();
}

void Widget::setDeCasteljauBezierDrawing()
{
	setBezierDrawing(DE_CASTELJAU_BEZIER_DRAWING);
	updatePlot();
}
