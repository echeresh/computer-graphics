#include <QPainter>
#include "quadranglemapper.h"
#include "translator.h"
#include "utils.h"

QuadrangleMapper::QuadrangleMapper()
{
}

QuadrangleMapper::QuadrangleMapper(const Utils::Quadrangle& quad, const QRectF& mediateRect) :
	quadrangle(quad)
{
	Q_ASSERT(quad.p0.y() == quad.p1.y() && quad.p2.y() == quad.p3.y());
	translator = Translator(Utils::Quadrangle(quad.p0,
											  quad.p1 + QPoint(1, 0),
											  quad.p2 + QPoint(0, 1),
											  quad.p3 + QPoint(1, 1)), mediateRect);
}

QPointF QuadrangleMapper::translate(const QPointF& p)
{
	return translator.translate(p);
}

bool QuadrangleMapper::isValid()
{
	return translator.isValid();
}

void QuadrangleMapper::draw(const MipMap& mipMap, QImage& buffer)
{
	if (!isValid())
	{
		return;
	}
	LineDrawer drawer0(quadrangle.p0, quadrangle.p2);
	LineDrawer drawer1(quadrangle.p1, quadrangle.p3);
	QPoint np0(0, quadrangle.p0.y() + 1);
	QPoint np1(np0);
	for (int y = quadrangle.p0.y(); y <= quadrangle.p2.y(); y++)
	{
		while (np0.y() != y)
		{
			if (drawer0.next(np0) == false)
			{
				drawer0 = LineDrawer(quadrangle.p1, quadrangle.p2);
			}
		}
		while (np1.y() != y)
		{
			if (drawer1.next(np1) == false)
			{
				drawer1 = LineDrawer(quadrangle.p2, quadrangle.p1);
			}
		}
		for (int x = qMin(np0.x(), np1.x()), xMax = qMax(np0.x(), np1.x()); x <= xMax; x++)
		{
			Utils::QuadrangleF quad =
			{
				translator.translate(QPointF(x + 0., y + 0.)),
				translator.translate(QPointF(x + 0., y + 1.)),
				translator.translate(QPointF(x + 1., y + 0.)),
				translator.translate(QPointF(x + 1., y + 1.)),
			};
			QPointF pixCenter(x + .5, y + .5);
			QRgb rgb = mipMap.map(translator.translate(pixCenter), quad);
			buffer.setPixel(x, y, rgb);
		}
	}
}

LineDrawer::LineDrawer(const QPoint& p0, const QPoint& p1) :
	p1(p1)
{
	dx = qAbs(p1.x() - p0.x());
	dy = qAbs(p1.y() - p0.y());
	sp = QPoint(Utils::sign(p1.x() - p0.x()), Utils::sign(p1.y() - p0.y()));
	ip = sp;
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
	p = p0;
	err = -dx / 2;
}

bool LineDrawer::next(QPoint& nextPoint)
{
	nextPoint = p;
	if (p == p1)
	{
		return false;
	}
	err += dy;
	p += ip;
	if (err > 0)
	{
		p += sp;
		err -= dx;
	}
	return true;
}
