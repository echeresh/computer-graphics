#ifndef UTILS_H
#define UTILS_H

#include <QVector3D>

namespace Utils
{
	struct Point3D
	{
		QVector3D coord;
		QPoint proj; //projection to 2D
		bool isVisible; //true if the point is behind projecting plane relative to camera
	};

	struct Line3D
	{
		Point3D p0;
		Point3D p1;
	};

	struct Triangle3D
	{
		Point3D p0;
		Point3D p1;
		Point3D p2;
	};

	template<class T>
	int sign(T v)
	{
		return v < 0 ? -1 : (v > 0 ? 1 : 0);
	}

	QVector3D min(const QVector3D& v0, const QVector3D& v1);
	QVector3D max(const QVector3D& v0, const QVector3D& v1);
	float bernsteinPolynomial(float u, int n, int i);
	QVector3D bezierValue(const QVector3D* controlPoints, int n, float t);
}
#endif // UTILS_H
