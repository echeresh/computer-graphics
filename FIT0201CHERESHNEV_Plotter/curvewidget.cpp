#include <QPainter>
#include <QMouseEvent>
#include "curvewidget.h"
#include "axisdrawer.h"
#include "curve.h"
#include "utils.h"

//axis constants
static const int MARK_HALF_LENGTH = 3;
static const QRgb AXIS_COLOR = qRgb(0xC0, 0xC0, 0xC0);
static const qreal MIN_UNIT_LENGTH = 10.;
static const qreal MAX_UNIT_LENGTH = 50.;

static const QPointF INIT_CENTER(0., 0.);
static const qreal INIT_SCALE = 20.;
static const qreal MAX_SCALE = 1e5;
static const qreal MIN_SCALE = 1e-1;
static const int INIT_THICKNESS = 1;
static const bool INIT_ANTIALIASING = false;
static const QRgb PLOT_COLOR = qRgb(0, 0, 0);

//rubber band constants
static const int DASH_SIZE = 5;
static const QRgb RUBBER_BAND_COLOR = qRgb(255, 0, 255);

CurveWidget::CurveWidget(QWidget *parent) :
	QWidget(parent),
	curve(*this)
{
	center = INIT_CENTER;
	scale = INIT_SCALE;
	thickness = INIT_THICKNESS;
	antiAliasing = INIT_ANTIALIASING;
	leftPressed = false;
	rightPressed = false;
	setColor(PLOT_COLOR);
}

void CurveWidget::setPixelXor(const QPoint& p, QRgb rgb)
{
	if (buffer.rect().contains(p))
	{
		QRgb oldRgb = buffer.pixel(p);
		QRgb xored = qRgb((qRed(rgb) ^ qRed(oldRgb)) & 0x000000FF,
						  (qGreen(rgb) ^ qGreen(oldRgb)) & 0x000000FF,
						  (qBlue(rgb) ^ qBlue(oldRgb)) & 0x000000FF);
		buffer.setPixel(p, xored);
	}
}

bool CurveWidget::contains(const QPoint& p)
{
	return buffer.rect().contains(p);
}

void CurveWidget::setPixel(const QPoint& p, QRgb rgb)
{
	if (buffer.rect().contains(p))
	{
		if (qRed(buffer.pixel(p)) > qRed(rgb))
		{
			buffer.setPixel(p, rgb);
		}
	}
}

void CurveWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.drawImage(0, 0, buffer);
}

void CurveWidget::resizeEvent(QResizeEvent*)
{
	buffer = QImage(size(), QImage::Format_RGB888);
	updatePlot();
}

QPointF CurveWidget::toRelative(const QPointF& absolute)
{
	QRectF viewPort = getViewPort();
	return QPointF((absolute.x() - viewPort.x()) * scale,
				   buffer.height() - (absolute.y() - viewPort.y()) * scale);
}

QPointF CurveWidget::toAbsolute(const QPointF& relative)
{
	QRectF viewPort = getViewPort();
	return QPointF(relative.x() / scale + viewPort.x(),
				   (buffer.height() - relative.y()) / scale + viewPort.y());
}

void CurveWidget::drawAxis()
{
	qreal axisScale = calcAxisScale();
	QRgb oldColor = setColor(AXIS_COLOR);
	AxisDrawer<qreal> axisDrawer(*this, axisScale, MARK_HALF_LENGTH);
	axisDrawer.draw(buffer.width(), buffer.height(), roundPoint(toRelative(QPoint())));
	setColor(oldColor);
	emit unitLengthChanged(QString::number(axisScale / scale));
}

void CurveWidget::mouseMoveEvent(QMouseEvent *mouseEvent)
{
	if (leftPressed || rightPressed)
	{
		drawRubberBand(mousePressBeginPoint, mousePressEndPoint, DASH_SIZE, RUBBER_BAND_COLOR);
		mousePressEndPoint = mouseEvent->pos();
		updatePlot();
	}
}

void CurveWidget::mousePressEvent(QMouseEvent *mouseEvent)
{
	if (mouseEvent->button() == Qt::LeftButton && rightPressed == false)
	{
		leftPressed = true;
		mousePressEndPoint = mousePressBeginPoint = mouseEvent->pos();
	}
	else if (mouseEvent->button() == Qt::RightButton && leftPressed == false)
	{
		rightPressed = true;
		mousePressEndPoint = mousePressBeginPoint = mouseEvent->pos();
	}
}

void CurveWidget::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
	if (mouseEvent->button() == Qt::LeftButton)
	{
		leftPressed = false;
		if (mousePressBeginPoint != mousePressEndPoint)
		{
			center = (toAbsolute(mouseEvent->pos()) + toAbsolute(mousePressBeginPoint)) / 2;
			qreal aspectRatio = static_cast<qreal>(buffer.width()) / buffer.height();
			qreal newScale;
			int aWidth = qAbs(mousePressBeginPoint.x() - mousePressEndPoint.x());
			int aHeight = qAbs(mousePressBeginPoint.y() - mousePressEndPoint.y());
			if (aspectRatio * aHeight > aWidth)
			{
				newScale = scale * (static_cast<qreal>(buffer.width()) / aWidth);
			}
			else
			{
				newScale = scale * (static_cast<qreal>(buffer.height()) / aHeight);
			}
			scale = qMin(newScale, MAX_SCALE);
		}
	}
	else if (mouseEvent->button() == Qt::RightButton)
	{
		rightPressed = false;
		if (mousePressBeginPoint != mousePressEndPoint)
		{
			center = (toAbsolute(mouseEvent->pos()) + toAbsolute(mousePressBeginPoint)) / 2;
			qreal aspectRatio = static_cast<qreal>(buffer.width()) / buffer.height();
			qreal newScale;
			int aWidth = qAbs(mousePressBeginPoint.x() - mousePressEndPoint.x());
			int aHeight = qAbs(mousePressBeginPoint.y() - mousePressEndPoint.y());
			if (aspectRatio * aHeight > aWidth)
			{
				newScale = scale / (static_cast<qreal>(buffer.width()) / aWidth);
			}
			else
			{
				newScale = scale / (static_cast<qreal>(buffer.height()) / aHeight);
			}
			scale = qMax(newScale, MIN_SCALE);
		}
	}
	updatePlot();
}

void CurveWidget::updatePlot()
{
	if (leftPressed || rightPressed)
	{
		drawRubberBand(mousePressBeginPoint, mousePressEndPoint, DASH_SIZE, RUBBER_BAND_COLOR);
	}
	else
	{
		buffer.fill(Qt::white);
		drawAxis();
		curve.draw(getViewPort(), scale, thickness, antiAliasing);
		QRgb oldColor = setColor(qRgb(0, 0, 0));
		drawRect(QPoint(), QPoint(buffer.width() - 1, buffer.height() - 1));
		setColor(oldColor);
	}
	const int FIELD_WIDTH = 2;
	emit thicknessChanged(QString("%1").arg(QString::number(thickness), FIELD_WIDTH));
	emit scaleChanged(QString("%1 : %2").
					  arg(QString::number(scale > 1 ? scale : 1)).
					  arg(QString::number(scale > 1 ? 1 : 1 / scale)));
	update();
}

void CurveWidget::updateAntiAliasing(bool antiAliasingValue)
{
	antiAliasing = antiAliasingValue;
	updatePlot();
}

void CurveWidget::updateThickness(int thicknessValue)
{
	thickness = thicknessValue;
	updatePlot();
}

void CurveWidget::resetScale()
{
	center = INIT_CENTER;
	scale = INIT_SCALE;
	updatePlot();
}

qreal CurveWidget::calcAxisScale()
{
	qreal axisScale = scale;
	while (axisScale > MAX_UNIT_LENGTH)
	{
		axisScale /= 10;
	}
	while (axisScale < MIN_UNIT_LENGTH)
	{
		axisScale *= 10;
	}
	return axisScale;
}

void CurveWidget::drawRubberBand(const QPoint& p0, const QPoint& p1, int dashSize, QRgb color)
{
	for (int x = qMin(p0.x(), p1.x()), xMax = qMax(p0.x(), p1.x()), pixCount = 0; x <= xMax; x++, pixCount++)
	{
		if (pixCount < dashSize)
		{
			setPixelXor(QPoint(x, p0.y()), color);
			setPixelXor(QPoint(x, p1.y()), color);
			continue;
		}
		pixCount = 0;

	}
	for (int y = qMin(p0.y(), p1.y()), yMax = qMax(p0.y(), p1.y()), pixCount = 0; y <= yMax; y++, pixCount++)
	{
		if (pixCount < dashSize)
		{
			setPixelXor(QPoint(p0.x(), y), color);
			setPixelXor(QPoint(p1.x(), y), color);
			continue;
		}
		pixCount = 0;
	}
}

QRectF CurveWidget::getViewPort()
{
	return QRectF(center.x() - buffer.width() / scale / 2,
				  center.y() - buffer.height() / scale / 2,
				  buffer.width() / scale,
				  buffer.height() / scale);
}
