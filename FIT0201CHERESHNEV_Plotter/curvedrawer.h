#ifndef CURVEDRAWER_H
#define CURVEDRAWER_H
#include "drawer.h"

class CurveDrawer : public Drawer
{
public:
	CurveDrawer();
	void setAntiAliasing(bool aaState);
	bool setPixel(const QPoint& point);
	QRgb pixelColor(qreal distance, qreal thickness);
	virtual bool setPixel(const QPoint& p, QRgb color) = 0;
	virtual QPointF toRelativeCoordinate(const QPointF &absolute) = 0;
	virtual QPointF toAbsoluteCoordinate(const QPointF &relative) = 0;
	virtual void fillCircle(const QPointF &center, qreal diameter);
	void drawLine(QPointF p0, QPointF p1, qreal thickness);
	virtual ~CurveDrawer();

private:
	bool antiAliasing;
};

#endif // CURVEDRAWER_H
