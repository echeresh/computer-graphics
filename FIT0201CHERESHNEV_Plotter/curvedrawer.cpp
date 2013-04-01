#include <qmath.h>
#include "curvedrawer.h"
#include "utils.h"

CurveDrawer::CurveDrawer() :
	antiAliasing(false)
{
}

void CurveDrawer::setAntiAliasing(bool aaState)
{
	antiAliasing = aaState;
}

QRgb CurveDrawer::pixelColor(qreal distance, qreal thickness)
{
	//returns color of pixel depending on distance from curve and its thickness
	//if antiAliasing is disabled then method returns just current color
	if (antiAliasing == false)
	{
		return color;
	}
	int v =  static_cast<int>(qMax(0., 255 * (distance - thickness + 1)));
	return qRgb(qMax(v, qRed(color)),
				qMax(v, qGreen(color)),
				qMax(v, qBlue(color)));
}

void CurveDrawer::fillCircle(const QPointF& center, qreal diameter)
{
	qreal radius = diameter / 2;
	for (int sy = -1; sy <= 1; sy += 2)
	{
		qreal d;
		for (QPoint p(Utils::roundPoint(center)); (d = qSqrt(Utils::normSquared(p - center)) - radius) <= 0; p.ry() += sy)
		{
			for (int sx = -1; sx <= 1; sx += 2)
			{
				for (QPoint pp(p); (d = qSqrt(Utils::normSquared(pp - center)) - radius) <= 0; pp.rx() += sx)
				{
					setPixel(pp, pixelColor(d, 0.));
				}
			}
		}
	}
}

void CurveDrawer::drawLine(QPointF p0, QPointF p1, qreal thickness)
{
	//draw line with thickness and squared ends
	struct DrawHelper
	{
		QPointF p0;
		QPointF p1;
		QPointF dp;
		qreal ed;
		DrawHelper(const QPointF& p0, const QPointF& p1)
		{
			this->p0 = p0;
			this->p1 = p1;
			dp = p1 - p0;
			ed = Utils::norm(dp);
		}
		double dist(const QPointF& p)
		{
			return qAbs((p.x() - p0.x()) * dp.y() - (p.y() - p0.y()) * dp.x()) / ed;
		}
		bool inside(const QPointF& p)
		{
			qreal v = (p.y() - p0.x()) * dp.y() + (p.x() - p1.y()) * dp.x();
			return v + (p1.y() - p0.x()) * dp.x() + (p0.x() - p0.y()) * dp.y() >= 0 &&
					v + (p1.y() - p1.x()) * dp.x() + (p0.x() - p1.y()) * dp.y() <= 0;
		}
		static QPointF makePoint(const QPointF& p)
		{
			return p;
		}
		static QPointF makePointSwap(const QPointF& p)
		{
			return QPointF(p.y(), p.x());
		}
	} drawHelper(p0, p1);
	QPointF (*pointMaker)(const QPointF& p) = DrawHelper::makePoint;
	if (qAbs(drawHelper.dp.y()) > qAbs(drawHelper.dp.x()))
	{
		p0 = DrawHelper::makePointSwap(p0);
		p1 = DrawHelper::makePointSwap(p1);
		pointMaker = DrawHelper::makePointSwap;
	}
	if (p1.x() < p0.x())
	{
		qSwap(p0, p1);
		qSwap(drawHelper.p0, drawHelper.p1);
		drawHelper.dp *= -1;
	}
	qreal xOffset = thickness * qAbs(p0.y() - p1.y()) / drawHelper.ed;
	QPointF dStep(1, (p1.y() - p0.y()) / (p1.x() - p0.x()));
	for (QPointF p(p0 - xOffset * dStep); p.x() <= p1.x() + xOffset; p += dStep)
	{
		for (int sy = -1; sy <= 1; sy += 2)
		{
			qreal dist;
			for (QPoint pp(Utils::roundPoint(p)); (dist = drawHelper.dist(pointMaker(pp))) <= thickness; pp.ry() += sy)
			{
				if (drawHelper.inside(pointMaker(pp)))
				{
					setPixel(Utils::roundPoint(pointMaker(pp)), pixelColor(dist, thickness));
				}
			}
		}
	}
}

void CurveDrawer::setPixel(const QPoint& p, QRgb rgb)
{
	//check red channel to avoid change "dark" pixel to "light"
	if (buffer.rect().contains(p) && qRed(buffer.pixel(p)) >= qRed(rgb))
	{
		buffer.setPixel(p, rgb);
	}
}

CurveDrawer::~CurveDrawer()
{
}
