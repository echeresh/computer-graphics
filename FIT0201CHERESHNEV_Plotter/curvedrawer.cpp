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
	int v = qMax(0., 255 * (distance - thickness + 1));
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
		for (QPoint p(roundPoint(center)); (d = qSqrt(sqr(p.x() - center.x()) + sqr(p.y() - center.y())) - radius) <= 0; p.ry() += sy)
		{
			for (int sx = -1; sx <= 1; sx += 2)
			{
				for (QPoint pp(p); (d = sqrt(sqr(pp.x() - center.x()) + sqr(p.y() - center.y())) - radius) <= 0; pp.rx() += sx)
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
	struct DistFinder
	{
		QPointF p0;
		QPointF p1;
		QPointF dp;
		qreal ed;
		DistFinder(const QPointF& p0, const QPointF& p1)
		{
			this->p0 = p0;
			this->p1 = p1;
			dp = p1 - p0;
			ed = norm(dp);
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
	} distFinder(p0, p1);
	QPointF (*pointMaker)(const QPointF& p) = makePoint;
	if (qAbs(distFinder.dp.y()) > qAbs(distFinder.dp.x()))
	{
		qSwap(p0.rx(), p0.ry());
		qSwap(p1.rx(), p1.ry());
		pointMaker = makePointSwap;
	}
	if (p1.x() < p0.x())
	{
		qSwap(p0, p1);
		qSwap(distFinder.p0, distFinder.p1);
		distFinder.dp *= -1;
	}
	qreal xOffset = thickness * qAbs(p0.y() - p1.y()) / distFinder.ed;
	QPointF dStep(1, (p1.y() - p0.y()) / (p1.x() - p0.x()));
	for (QPointF p(p0 - xOffset * dStep); p.x() <= p1.x() + xOffset; p += dStep)
	{
		for (int sy = -1; sy <= 1; sy += 2)
		{
			qreal dist;
			for (QPoint pp(roundPoint(p)); (dist = distFinder.dist(pointMaker(pp))) <= thickness; pp.ry() += sy)
			{
				if (distFinder.inside(pointMaker(pp)))
				{
					setPixel(roundPoint(pointMaker(pp)), pixelColor(dist, thickness));
				}
			}
		}
	}
}

CurveDrawer::~CurveDrawer()
{
}
