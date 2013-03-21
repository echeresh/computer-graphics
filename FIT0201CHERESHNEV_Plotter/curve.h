#ifndef CURVE_H
#define CURVE_H
#include <QRectF>
#include "curvedrawer.h"

struct DrawContext
{
	DrawContext(const QRectF& viewPort, CurveDrawer &drawer, qreal scale, qreal thickness, bool antiAliasing);

	CurveDrawer &drawer;
	const QRectF viewPort;
	const qreal scale;
	const qreal thickness;
	const bool antiAliasing;
};

class CurveBranch;

class Curve
{
public:
	Curve(CurveDrawer &drawer);
	void draw(const QRectF& viewPort, qreal scale, qreal thickness, bool antiAliasing);
	~Curve();

	qreal calcX(qreal t) const;
	qreal calcY(qreal t) const;
	qreal calcXDerivative(qreal t) const;
	qreal calcYDerivative(qreal t) const;

private:
	QVector<CurveBranch*> branches;
	CurveDrawer &drawer;
};

class CurveBranch
{
public:
	CurveBranch(qreal tMin, qreal tMax, bool xGrowthDirection, bool yGrowthDirection, const Curve &curve);
	void draw(const DrawContext &drawContext);

private:
	qreal findT(qreal value, qreal (Curve::*f)(qreal) const);
	QPair<qreal, qreal> findRange(const QRectF& viewPort);
	void draw(qreal t0, qreal t1, const DrawContext& drawContext);
	qreal findStep(qreal t, qreal t1, qreal unitLength);
	qreal calcStep(qreal t0, qreal t1, qreal unitLength);

private:
	qreal tMin;
	qreal tMax;
	const Curve &curve;
	DrawContext& drawContext;
	qreal (QRectF::*xMin)() const;
	qreal (QRectF::*xMax)() const;
	qreal (QRectF::*yMin)() const;
	qreal (QRectF::*yMax)() const;
};
#endif // CURVE_H
