#ifndef CURVE_H
#define CURVE_H
#include <QRectF>
#include "curvedrawer.h"

struct DrawContext
{
	DrawContext(const QRectF& viewPort, CurveDrawer &drawer, qreal scale, qreal thickness, bool antiAliasing);

	CurveDrawer& drawer;
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
	~Curve();

	void draw(const QRectF& viewPort, qreal scale, qreal thickness, bool antiAliasing);
    static qreal calcX(qreal t);
    static qreal calcY(qreal t);
    static qreal calcXDerivative(qreal t);
    static qreal calcYDerivative(qreal t);

private:
	QVector<CurveBranch*> branches;
	CurveDrawer &drawer;
};

class CurveBranch
{
public:
	CurveBranch(qreal tMin, qreal tMax, const Curve& curve);
	void draw(const DrawContext& drawContext);

private:
    qreal findT(qreal value, qreal (*f)(qreal));
	QPair<qreal, qreal> findRange(const QRectF& viewPort);
	qreal findStep(qreal t, qreal t1, qreal unitLength);
	qreal calcStep(qreal t0, qreal t1, qreal unitLength);

private:
	qreal tMin;
	qreal tMax;
	const Curve& curve;
	DrawContext& drawContext;
};
#endif // CURVE_H
