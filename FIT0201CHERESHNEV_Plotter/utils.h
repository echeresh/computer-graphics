#ifndef UTILS_H
#define UTILS_H
#include <QPoint>
#include <QPointF>
#include <qmath.h>

template<class T>
T sqr(T value)
{
	return value * value;
}

template<class T>
int sign(T value)
{
	return value > 0 ? 1 : (value < 0 ? -1 : 0);
}

inline qreal norm(const QPointF &point)
{
	return qSqrt(sqr(point.x()) + sqr(point.y()));
}

inline QPoint roundPoint(const QPointF &point)
{
	return QPoint(static_cast<int>(point.x() + .5), static_cast<int>(point.y() + .5));
}

inline QPointF makePoint(const QPointF &p)
{
	return p;
}

inline QPointF makePointSwap(const QPointF &p)
{
	return QPointF(p.y(), p.x());
}
#endif // UTILS_H
