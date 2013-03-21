#include <exception>
#include "utils.h"
#include "drawer.h"

Drawer::Drawer() :
	color(qRgb(0, 0, 0))
{
}

QRgb Drawer::setColor(QRgb color)
{
	QRgb oldColor = this->color;
	this->color = color;
	return oldColor;
}

bool Drawer::setPixel(const QPoint& p)
{
	return setPixel(p, color);
}

void Drawer::drawLine(const QPoint& p0, const QPoint& p1)
{
	int dx = qAbs(p1.x() - p0.x());
	int dy = qAbs(p1.y() - p0.y());
	QPoint sp(sign(p1.x() - p0.x()), sign(p1.y() - p0.y()));
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
		setPixel(p, color);
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

void Drawer::drawLine(const QPoint& p0, const QPoint& p1, int thickness)
{
	int dx = qAbs(p1.x() - p0.x());
	int dy = qAbs(p1.y() - p0.y());
	QPoint sp(sign(p1.x() - p0.x()), sign(p1.y() - p0.y()));
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
		fillCircle(p, thickness);
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

void Drawer::fillRect(const QPoint& p0, const QPoint& p1)
{
	for (int x = qMin(p0.x(), p1.x()), maxX = qMax(p0.x(), p1.x()); x <= maxX; x++)
	{
		drawLine(QPoint(x, p0.y()), QPoint(x, p1.y()));
	}
}

void Drawer::fillCircle(const QPoint& center, int diameter)
{
	diameter *= diameter;
	for (int sy = -1; sy <= 1; sy += 2)
	{
		for (QPoint p(roundPoint(center)); 4 * (sqr(p.x() - center.x()) + sqr(p.y() - center.y())) <= diameter; p.ry() += sy)
		{
			setPixel(p);
			for (int sx = -1; sx <= 1; sx += 2)
			{
				for (QPoint pp(p); 4 * (sqr(pp.x() - center.x()) + sqr(pp.y() - center.y())) <= diameter; pp.rx() += sx)
				{
					setPixel(pp);
				}
			}
		}
	}
}

void Drawer::drawRect(const QPoint& p0, const QPoint& p1)
{
	drawLine(p0, QPoint(p0.x(), p1.y()));
	drawLine(p0, QPoint(p1.x(), p0.y()));
	drawLine(p1, QPoint(p0.x(), p1.y()));
	drawLine(p1, QPoint(p1.x(), p0.y()));
}
