#include <QVector3D>
#include "utils.h"

namespace Utils
{
	Line3D makeLine3D(const QVector3D& p0, const QVector3D& p1)
	{
		return qMakePair(p0, p1);
	}

	QVector3D min(const QVector3D& v0, const QVector3D& v1)
	{
		return QVector3D(qMin(v0.x(), v1.x()), qMin(v0.y(), v1.y()), qMin(v0.z(), v1.z()));
	}

	QVector3D max(const QVector3D& v0, const QVector3D& v1)
	{
		return QVector3D(qMax(v0.x(), v1.x()), qMax(v0.y(), v1.y()), qMax(v0.z(), v1.z()));
	}
}
