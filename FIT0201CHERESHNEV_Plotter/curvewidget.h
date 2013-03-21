#ifndef CURVEWIDGET_H
#define CURVEWIDGET_H
#include <QWidget>
#include <QImage>
#include "curvedrawer.h"
#include "curve.h"

class CurveWidget : public QWidget, CurveDrawer
{
	Q_OBJECT
public:
	explicit CurveWidget(QWidget *parent = 0);
	void paintEvent(QPaintEvent*);
	void resizeEvent(QResizeEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);

	bool setPixelXor(const QPoint& p, QRgb rgb);
	bool setPixel(const QPoint& p, QRgb rgb);
	QPointF toRelative(const QPointF& absolute);
	QPointF toAbsolute(const QPointF& relative);

signals:
	void unitLengthChanged(const QString& unitLengthText);
	void thicknessChanged(const QString& thicknessText);
	void scaleChanged(const QString& scaleText);

private slots:
	void updateAntiAliasing(bool antiAliasingValue);
	void updateThickness(int thicknessValue);
	void resetScale();

private:
	void updatePlot();
	void drawAxis();
	QRectF getViewPort();
	qreal calcAxisScale();
	void drawRubberBand(const QPoint& p0, const QPoint& p1, int dashSize, QRgb color);

private:
	QPointF center;
	QImage buffer;
	QPoint mousePressBeginPoint;
	QPoint mousePressEndPoint;
	bool leftPressed;
	bool rightPressed;
	qreal scale;
	int thickness;
	bool antiAliasing;
	Curve curve;
};

#endif // CURVEWIDGET_H
