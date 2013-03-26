#include <QPainter>
#include <QColor>
#include <QMouseEvent>
#include "cassini.h"
#include "utils.h"
#include "cassiniwidget.h"
#include "axisdrawer.h"

static const int X0 = -100;
static const int Y0 = 0;
static const int X1 = 100;
static const int Y1 = 0;
static const int R0 = 10000;
static const int INIT_THICKNESS = 1;
static const int UNIT_LENGTH = 10;
static const int MARK_HALF_LENGTH = 3;
static const QRgb AXIS_COLOR = qRgb(0xC0, 0xC0, 0xC0);
static const QRgb FOCUS_COLOR = qRgb(255, 0, 0);
static const QRgb PLOT_COLOR = qRgb(0, 0, 0);

CassiniWidget::CassiniWidget(QWidget *parent) :
	QWidget(parent),
	focus0(X0, Y0),
	focus1(X1, Y1)
{
	R = R0;
	thickness = INIT_THICKNESS;
	isMousePressed = false;
	setColor(PLOT_COLOR);
}

void CassiniWidget::drawAxis()
{
	QRgb oldColor = setColor(AXIS_COLOR);
	AxisDrawer<int> axisDrawer(*this, UNIT_LENGTH, MARK_HALF_LENGTH);
	axisDrawer.draw(buffer.width(), buffer.height(), QPoint(buffer.width() / 2, buffer.height() / 2));
	setColor(oldColor);
}

bool CassiniWidget::contains(const QPoint& p)
{
	return buffer.rect().contains(p);
}

void CassiniWidget::setPixel(const QPoint& p, QRgb rgb)
{
	if (buffer.rect().contains(p))
	{
		buffer.setPixel(p, rgb);
	}
}

void CassiniWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	painter.drawImage(0, 0, buffer);
}

void CassiniWidget::resizeEvent(QResizeEvent *)
{
	buffer = QImage(size(), QImage::Format_RGB888);
	updatePlot();
}

void CassiniWidget::mouseMoveEvent(QMouseEvent *mouseEvent)
{
	if (isMousePressed)
	{
		focus1 = toAbsolute(mouseEvent->pos());
		updatePlot();
	}
}

void CassiniWidget::mousePressEvent(QMouseEvent *mouseEvent)
{
	if (mouseEvent->button() == Qt::LeftButton)
	{
		isMousePressed = true;
		focus0 = toAbsolute(mouseEvent->pos());
	}
}

void CassiniWidget::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
	if (mouseEvent->button() == Qt::LeftButton && isMousePressed)
	{
		isMousePressed = false;
		focus1 = toAbsolute(mouseEvent->pos());
		updatePlot();
	}
}

QPoint CassiniWidget::toRelative(const QPoint& point)
{
	return QPoint(buffer.width() / 2 + point.x(), buffer.height() / 2 - point.y());
}

QPoint CassiniWidget::toAbsolute(const QPoint& point)
{
	return QPoint(point.x() - buffer.width() / 2, buffer.height() / 2 - point.y());
}

void CassiniWidget::drawFocusLine(const QPoint& f0, const QPoint& f1)
{
	drawLine(f0, f1);
}

void CassiniWidget::updatePlot()
{
	QPoint f0 = toRelative(focus0);
	QPoint f1 = toRelative(focus1);
	buffer.fill(Qt::white);
	drawAxis();
	QRgb oldColor = setColor(FOCUS_COLOR);
	QPoint off(1, 1);
	fillRect(f0 - off, f0 + off);
	fillRect(f1 - off, f1 + off);
	setColor(oldColor);
	if (isMousePressed)
	{
		drawFocusLine(f0, f1);
	}
	else
	{
		Cassini(f0, f1, R, *this, thickness).draw();
	}
	oldColor = setColor(qRgb(0, 0, 0));
	drawRect(QPoint(), QPoint(buffer.width() - 1, buffer.height() - 1));
	setColor(oldColor);

	const int FIELD_WIDTH = 4;
	emit focus0Changed(QString("%1;%2").
					   arg(QString::number(focus0.x()), FIELD_WIDTH).
					   arg(QString::number(focus0.y()), FIELD_WIDTH));
	emit focus1Changed(QString("%1;%2").
					   arg(QString::number(focus1.x()), FIELD_WIDTH).
					   arg(QString::number(focus1.y()), FIELD_WIDTH));
	emit RChanged(QString("%1").arg(QString::number(R), FIELD_WIDTH));
	emit thicknessChanged(QString::number(thickness));
	update();
}

void CassiniWidget::updateR(int RValue)
{
	R = RValue;
	updatePlot();
}

void CassiniWidget::updateThickness(int thicknessValue)
{
	thickness = thicknessValue;
	updatePlot();
}
