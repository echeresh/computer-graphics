#ifndef UTLS_H
#define UTLS_H

#include <QColor>
#include <QPointF>

namespace Utils
{
	struct QuadrangleF
	{
		QPointF p0;
		QPointF p1;
		QPointF p2;
		QPointF p3;

		qreal maxSide() const;
		QuadrangleF scaleTo(const QSize& size) const;
	};

	struct Quadrangle
	{
		QPoint p0;
		QPoint p1;
		QPoint p2;
		QPoint p3;

		Quadrangle();
		Quadrangle(const QPoint& p0, const QPoint& p1, const QPoint& p2, const QPoint& p3);
	};

	struct RgbF
	{
		qreal r;
		qreal g;
		qreal b;

		RgbF(QRgb rgb);
		RgbF(qreal r, qreal g, qreal b);
		QRgb toRgb() const;
	};

	template<class T>
	int sign(T v)
	{
		return v < 0 ? -1 : (v > 0 ? 1 : 0);
	}

	RgbF bilinearFiltering(qreal ratio, const RgbF& rgb0, const RgbF& rgb1);
	QVector<double> solveSLE(const QVector<QVector<qreal> >& sle);
	QPointF mirrorPoint(const QPointF& point);
}
#endif // UTLS_H
