#include <QVector>
#include <qmath.h>
#include <limits.h>
#include "curve.h"
#include "utils.h"

static const qreal INF = std::numeric_limits<qreal>::max();
static const qreal EPS = std::numeric_limits<qreal>::epsilon();

DrawContext::DrawContext(const QRectF& viewPort, CurveDrawer &drawer, qreal scale, qreal thickness, bool antiAliasing) :
	viewPort(viewPort),
	drawer(drawer),
	scale(scale),
	thickness(thickness),
	antiAliasing(antiAliasing)
{
}

Curve::Curve(CurveDrawer &drawer) :
	drawer(drawer)
{
	//on every CurveBranch x and y always increases/decreases
	branches.append(new CurveBranch(-INF, -1 - qSqrt(2), true, true, *this));
	branches.append(new CurveBranch(-1 - qSqrt(2), -1 - EPS, true, false, *this));

	branches.append(new CurveBranch(1 + EPS, INF, false, true, *this));

	branches.append(new CurveBranch(-1 + EPS, 0, true, false, *this));
	branches.append(new CurveBranch(0, qSqrt(2) - 1, false, false, *this));
	branches.append(new CurveBranch(qSqrt(2) - 1, 1 - EPS, false, true, *this));
}

void Curve::draw(const QRectF& viewPort, qreal scale, qreal thickness, bool antiAliasing)
{
	DrawContext drawContext(viewPort, drawer, scale, thickness, antiAliasing);
	for (QVector<CurveBranch*>::Iterator it = branches.begin(); it != branches.end(); it++)
	{
		(*it)->draw(drawContext);
	}
}

Curve::~Curve()
{
	for (QVector<CurveBranch*>::Iterator it = branches.begin(); it != branches.end(); it++)
	{
		delete (*it);
	}
}

qreal Curve::calcX(qreal t) const
{
	return t * t / (t * t - 1);
}

qreal Curve::calcY(qreal t) const
{
	return (t * t + 1) / (t + 1);
}

qreal Curve::calcXDerivative(qreal t) const
{
	return -2 * t / sqr(t * t - 1);
}

qreal Curve::calcYDerivative(qreal t) const
{
	return (t * t + 2 * t - 1) / sqr(t + 1);
}

CurveBranch::CurveBranch(qreal tMin, qreal tMax, bool xGrowthDirection, bool yGrowthDirection, const Curve &curve) :
	tMin(tMin),
	tMax(tMax),
	curve(curve),
	drawContext(drawContext)
{
	//xGrowthDirection yGrowthDirection are used to define
	//growth direction of x,y on the interval [tMin; tMax]
	if (xGrowthDirection)
	{
		xMin = &QRectF::left;
		xMax = &QRectF::right;
	}
	else
	{
		xMin = &QRectF::right;
		xMax = &QRectF::left;
	}
	if (yGrowthDirection)
	{
		yMin = &QRectF::top;
		yMax = &QRectF::bottom;
	}
	else
	{
		yMin = &QRectF::bottom;
		yMax = &QRectF::top;
	}
}

qreal CurveBranch::findT(qreal value, qreal (Curve::*f)(qreal) const)
{
	const qreal MAX_ERR_EPS = 1e-6;
	qreal f_tMin = (curve.*f)(tMin);
	qreal f_tMax = (curve.*f)(tMax);
	bool asc = f_tMin < f_tMax;
	if (value > f_tMin && value > f_tMax)
	{
		return asc ? tMax : tMin;
	}
	if (value < f_tMin && value < f_tMax)
	{
		return asc ? tMin : tMax;
	}
	for(qreal lt = tMin, rt = tMax; ;)
	{
		qreal mt = (lt + rt) / 2;
		qreal fmt = (curve.*f)(mt);
		if (qAbs(value - fmt) < MAX_ERR_EPS)
		{
			return mt;
		}
		if (value > fmt)
		{
			(asc ? lt : rt) = mt;
		}
		else
		{
			(asc ? rt : lt) = mt;
		}
	}
}

QPair<qreal, qreal> CurveBranch::findRange(const QRectF& viewPort)
{
	qreal txMin = findT((viewPort.*xMin)(), &Curve::calcX);
	qreal txMax = findT((viewPort.*xMax)(), &Curve::calcX);
	qreal tyMin = findT((viewPort.*yMin)(), &Curve::calcY);
	qreal tyMax = findT((viewPort.*yMax)(), &Curve::calcY);
	if (txMin > tyMax || tyMin > txMax)
	{
		return qMakePair(txMin, txMin);
	}
	return qMakePair(qMax(txMin, tyMin), qMin(txMax, tyMax));
}

qreal CurveBranch::calcStep(qreal t0, qreal t1, qreal unitLength)
{
	qreal Mx = qMax(qAbs(curve.calcXDerivative(t0)), qAbs(curve.calcXDerivative(t1)));
	qreal My = qMax(qAbs(curve.calcYDerivative(t0)), qAbs(curve.calcYDerivative(t1)));
	return unitLength / qSqrt(sqr(Mx) + sqr(My));
}

qreal CurveBranch::findStep(qreal t, qreal t1, qreal unitLength)
{
	//Greedy algorithm : doubling + dichotomy while it is possible
	const qreal START_EPS = 1e-9;
	const qreal CMP_EPS = 1e-6;
	qreal eps, h;
	for(eps = qMin(START_EPS, (t1 - t) / 2); t + eps <= t1;)
	{
		if ((h = calcStep(t, qMin(t + eps, t1), unitLength)) <= eps)
		{
			return h;
		}
		else
		{
			eps *= 2;
		}
	}
	for(eps /= 2; t1 - (t + eps) > CMP_EPS;)
	{
		if ((h = calcStep(t, qMin(t + eps, t1), unitLength)) <= eps)
		{
			return h;
		}
		else
		{
			eps = (t1 + (t + eps)) / 2 - t;
		}
	}
	return t1 - t;
}

void CurveBranch::draw(qreal t0, qreal t1, const DrawContext& drawContext)
{
	const int ANTIALIASING_STEP = 3;
	const bool PLAIN_DRAWING = qAbs(drawContext.thickness - 1.) < EPS;

	CurveDrawer& drawer = drawContext.drawer;
	drawer.setAntiAliasing(drawContext.antiAliasing);
	QPointF absolutePoint(curve.calcX(t0), curve.calcY(t0));
	QPointF prevPoint = drawer.toRelative(absolutePoint);
	drawer.fillCircle(prevPoint, drawContext.thickness);
	for (qreal t = t0, h; t < t1; t += h)
	{
		h = findStep(t, t1, 1 / drawContext.scale);
		absolutePoint = QPointF(curve.calcX(t), curve.calcY(t));
		QPointF curPoint = drawer.toRelative(absolutePoint);
		QPoint drawerPoint = roundPoint(curPoint);

		if (PLAIN_DRAWING)
		{
			((Drawer&)drawer).setPixel(drawerPoint);
		}
		else if (drawer.contains(drawerPoint) &&
				(curPoint - prevPoint).manhattanLength() > ANTIALIASING_STEP)
		{
			drawer.drawLine(prevPoint, curPoint, drawContext.thickness / 2);
			drawer.fillCircle(curPoint, drawContext.thickness);
			prevPoint = curPoint;	
		}
	}
	if (PLAIN_DRAWING == false)
	{
		QPointF lastPoint = drawer.toRelative(QPointF(curve.calcX(t1), curve.calcY(t1)));
		drawer.drawLine(prevPoint, lastPoint, drawContext.thickness / 2);
		drawer.fillCircle(lastPoint, drawContext.thickness);
	}
}

void CurveBranch::draw(const DrawContext& drawContext)
{
	QPair<qreal, qreal> pair = findRange(drawContext.viewPort);
	draw(pair.first, pair.second, drawContext);
}
