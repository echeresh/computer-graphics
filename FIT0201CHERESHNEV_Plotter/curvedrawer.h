#ifndef CURVEDRAWER_H
#define CURVEDRAWER_H
#include "drawer.h"

class CurveDrawer : public Drawer
{
public:
	CurveDrawer();
	void setAntiAliasing(bool aaState);
	QRgb pixelColor(qreal distance, qreal thickness);
    void drawLine(QPointF p0, QPointF p1, qreal thickness);
    void setPixel(const QPoint& p, QRgb rgb);

	virtual QPointF toRelative(const QPointF& absolute) = 0;
	virtual QPointF toAbsolute(const QPointF& relative) = 0;
    virtual void fillCircle(const QPointF& center, qreal diameter);
	virtual ~CurveDrawer();

	using Drawer::setPixel; //otherwise Drawer::setPixel(const QPoint& p) will be hidden by CurveDrawer::setPixel(const QPoint& p, QRgb rgb)

private:
	bool antiAliasing;
};

#endif // CURVEDRAWER_H
