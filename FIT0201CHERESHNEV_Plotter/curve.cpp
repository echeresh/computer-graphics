#include <QVector>
#include <qmath.h>
#include <cfloat>
#include "curve.h"
#include "utils.h"

static const qreal INF = DBL_MAX;
static const qreal EPS = DBL_EPSILON;

DrawContext::DrawContext(const QRectF& viewPort, CurveDrawer &drawer, qreal scale, qreal thickness, bool antiAliasing) :
    drawer(drawer),
	viewPort(viewPort),
	scale(scale),
	thickness(thickness),
	antiAliasing(antiAliasing)
{
}

Curve::Curve(CurveDrawer &drawer) :
	drawer(drawer)
{
	//on every CurveBranch x and y always increases/decreases
	branches.append(new CurveBranch(-INF, -1 - qSqrt(2), *this));
	branches.append(new CurveBranch(-1 - qSqrt(2), -1 - EPS, *this));

	branches.append(new CurveBranch(1 + EPS, INF, *this));

	branches.append(new CurveBranch(-1 + EPS, 0, *this));
	branches.append(new CurveBranch(0, qSqrt(2) - 1, *this));
	branches.append(new CurveBranch(qSqrt(2) - 1, 1 - EPS, *this));
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

qreal Curve::calcX(qreal t)
{
	return t * t / (t * t - 1);
}

qreal Curve::calcY(qreal t)
{
	return (t * t + 1) / (t + 1);
}

qreal Curve::calcXDerivative(qreal t)
{
	return -2 * t / qPow(t * t - 1, 2.);
}

qreal Curve::calcYDerivative(qreal t)
{
	return (t * t + 2 * t - 1) / qPow(t + 1, 2.);
}

CurveBranch::CurveBranch(qreal tMin, qreal tMax, const Curve &curve) :
	tMin(tMin),
	tMax(tMax),
	curve(curve),
	drawContext(drawContext)
{
}

qreal CurveBranch::findT(qreal value, qreal (*f)(qreal))
{
	const qreal MAX_ERR_EPS = 1e-6;
	qreal f_tMin = f(tMin);
	qreal f_tMax = f(tMax);
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
		qreal fmt = f(mt);
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
	qreal txMin = findT(viewPort.left(), Curve::calcX);
	qreal txMax = findT(viewPort.right(), Curve::calcX);
	qreal tyMin = findT(viewPort.top(), Curve::calcY);
	qreal tyMax = findT(viewPort.bottom(), Curve::calcY);
	if (txMin > txMax)
	{
		qSwap(txMin, txMax);
	}
	if (tyMin > tyMax)
	{
		qSwap(tyMin, tyMax);
	}
	if (txMin > tyMax || tyMin > txMax)
	{
		return qMakePair(txMin, txMin);
	}
	return qMakePair(qMax(txMin, tyMin), qMin(txMax, tyMax));
}

qreal CurveBranch::calcStep(qreal t0, qreal t1, qreal unitLength)
{
	qreal Mx = qMax(qAbs(Curve::calcXDerivative(t0)), qAbs(Curve::calcXDerivative(t1)));
	qreal My = qMax(qAbs(Curve::calcYDerivative(t0)), qAbs(Curve::calcYDerivative(t1)));
	return unitLength / qSqrt(Mx * Mx + My * My);
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

void CurveBranch::draw(const DrawContext& drawContext)
{
	const int ANTIALIASING_STEP = 3;
	const bool PLAIN_DRAWING = qAbs(drawContext.thickness - 1.) < EPS;
	
	const QPair<qreal, qreal> range = findRange(drawContext.viewPort);
	const qreal t0 = range.first;
	const qreal t1 = range.second;
	
	CurveDrawer& drawer = drawContext.drawer;
	drawer.setAntiAliasing(drawContext.antiAliasing);
	
    QPointF absolutePoint(Curve::calcX(t0), Curve::calcY(t0));
	QPointF prevPoint = drawer.toRelative(absolutePoint);
	drawer.fillCircle(prevPoint, drawContext.thickness);
	for (qreal t = t0, h; t < t1; t += h)
	{
		h = findStep(t, t1, 1 / drawContext.scale);
		absolutePoint = QPointF(Curve::calcX(t), Curve::calcY(t));
		QPointF curPoint = drawer.toRelative(absolutePoint);
		QPoint drawerPoint = Utils::roundPoint(curPoint);

		if (PLAIN_DRAWING)
		{
			drawer.setPixel(drawerPoint);
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
		QPointF lastPoint = drawer.toRelative(QPointF(Curve::calcX(t1), Curve::calcY(t1)));
		drawer.drawLine(prevPoint, lastPoint, drawContext.thickness / 2);
		drawer.fillCircle(lastPoint, drawContext.thickness);
	}
}
