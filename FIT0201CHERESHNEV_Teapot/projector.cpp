#include "projector.h"
#include "utils.h"

void Projector::drawLine(const QPoint& p0, const QPoint& p1)
{
	//Bresenham's line algorithm
	int dx = qAbs(p1.x() - p0.x());
	int dy = qAbs(p1.y() - p0.y());
	QPoint sp(Utils::sign(p1.x() - p0.x()), Utils::sign(p1.y() - p0.y()));
	QPoint ip(sp);
	if (dx > dy)
	{
		ip.setY(0);
		sp.setX(0);
	}
	else
	{
		qSwap(dx, dy);
		ip.setX(0);
		sp.setY(0);
	}
	QPoint p(p0);
	for(int err = -dx / 2; ;)
	{
		setPixel(p);
		if (p == p1)
		{
			return;
		}
		err += dy;
		p += ip;
		if (err > 0)
		{
			p += sp;
			err -= dx;
		}
	}
}

void Projector::drawLine(const QVector3D& v0, const QVector3D& v1)
{
	QPoint p0 = getProjection(v0);
	QPoint p1 = getProjection(v1);
	bool visible0 = !p0.isNull();
	bool visible1 = !p1.isNull();
	if (visible0 && visible1)
	{
		drawLine(p0, p1);
	}
	else if (!visible0 && visible1)
	{
		drawLine(getIntersection(v0, v1), p1);
	}
	else if (visible0 && !visible1)
	{
		drawLine(getIntersection(v0, v1), p0);
	}
}

void Projector::resizeBuffer(const QSize &newSize)
{
	buffer = QImage(newSize, QImage::Format_RGB888);
}


void Projector::setPixel(const QPoint& p)
{
	if (buffer.rect().contains(p))
	{
		buffer.setPixel(p, rgb);
	}
}

void Projector::setColor(QRgb newRgb)
{
	rgb = newRgb;
}
