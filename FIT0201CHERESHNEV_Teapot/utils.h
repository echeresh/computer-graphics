#ifndef UTILS_H
#define UTILS_H

#include <QPair>
#include <QVector3D>
namespace Utils
{
	typedef QPair<QVector3D, QVector3D> Line3D;

	Line3D makeLine3D(const QVector3D& p0, const QVector3D& p1);
	QVector3D min(const QVector3D& v0, const QVector3D& v1);
	QVector3D max(const QVector3D& v0, const QVector3D& v1);

	template<class T>
	int sign(T v)
	{
		return v < 0 ? -1 : (v > 0 ? 1 : 0);
	}
}
#endif // UTILS_H
