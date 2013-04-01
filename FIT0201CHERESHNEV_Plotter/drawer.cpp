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

void Drawer::setPixel(const QPoint& p)
{
	setPixel(p, color);
}

void Drawer::setPixelXor(const QPoint& p, QRgb rgb)
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

void Drawer::drawLine(const QPoint& p0, const QPoint& p1, int thickness)
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
	for (int x = qMin(p0.x(), p1.x()), xMax = qMax(p0.x(), p1.x()); x <= xMax; x++)
	{
		drawLine(QPoint(x, p0.y()), QPoint(x, p1.y()));
	}
}

void Drawer::fillCircle(const QPoint& center, int diameter)
{
	diameter *= diameter;
	setPixel(center);
	for (int sy = -1; sy <= 1; sy += 2)
	{
		for (QPoint p(center + QPoint(0, 1)); 4 * Utils::normSquared(p - center) <= diameter; p.ry() += sy)
		{
			setPixel(p);
			for (int sx = -1; sx <= 1; sx += 2)
			{
				for (QPoint pp(p + QPoint(1, 0)); 4 * Utils::normSquared(pp - center) <= diameter; pp.rx() += sx)
				{
					setPixel(pp);
				}
			}
		}
	}
}

void Drawer::resizeBuffer(const QSize &newSize)
{
	buffer = QImage(newSize, QImage::Format_RGB888);
}

void Drawer::setPixel(const QPoint& p, QRgb rgb)
{
	if (buffer.rect().contains(p))
	{
		buffer.setPixel(p, rgb);
	}
}

bool Drawer::contains(const QPoint& p)
{
	return buffer.rect().contains(p);
}

void Drawer::drawRect(const QPoint& p0, const QPoint& p1)
{
	drawLine(p0, QPoint(p0.x(), p1.y()));
	drawLine(p0, QPoint(p1.x(), p0.y()));
	drawLine(p1, QPoint(p0.x(), p1.y()));
	drawLine(p1, QPoint(p1.x(), p0.y()));
}

Drawer::~Drawer()
{
}
