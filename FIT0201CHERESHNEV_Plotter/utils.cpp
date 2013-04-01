#include <qmath.h>
#include "utils.h"

namespace Utils
{
	int sign(int value)
	{
		return value > 0 ? 1 : (value < 0 ? -1 : 0);
	}

	qreal norm(const QPointF& point)
	{
		return qSqrt(normSquared(point));
	}

	int normSquared(const QPoint& point)
	{
		return point.x() * point.x() + point.y() * point.y();
	}

	qreal normSquared(const QPointF& point)
	{
		return point.x() * point.x() + point.y() * point.y();
	}

	QPoint roundPoint(const QPointF& point)
	{
		return QPoint(static_cast<int>(point.x() + .5), static_cast<int>(point.y() + .5));
	}
}
