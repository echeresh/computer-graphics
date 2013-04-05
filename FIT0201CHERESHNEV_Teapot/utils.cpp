#include <QVector3D>
#include <qmath.h>
#include "utils.h"

namespace Utils
{
	static const int BIN_COEFS_MAX = 5;
	static const int BIN_COEFS[BIN_COEFS_MAX][BIN_COEFS_MAX] =
	{
		{1, 0, 0, 0, 0},
		{1, 1, 0, 0, 0},
		{1, 2, 1, 0, 0},
		{1, 3, 3, 1, 0},
		{1, 4, 6, 4, 1},
	};

	QVector3D min(const QVector3D& v0, const QVector3D& v1)
	{
		return QVector3D(qMin(v0.x(), v1.x()), qMin(v0.y(), v1.y()), qMin(v0.z(), v1.z()));
	}

	QVector3D max(const QVector3D& v0, const QVector3D& v1)
	{
		return QVector3D(qMax(v0.x(), v1.x()), qMax(v0.y(), v1.y()), qMax(v0.z(), v1.z()));
	}

	float bernsteinPolynomial(float u, int n, int i)
	{
		Q_ASSERT(n >= 0 && i >= 0 && n <= BIN_COEFS_MAX && i <= n);
		return BIN_COEFS[n][i] * qPow(static_cast<float>(u), static_cast<float>(i)) *
				qPow(static_cast<float>(1 - u), static_cast<float>(n - i));
	}

	QVector3D bezierValue(const QVector3D* controlPoints, int n, float t)
	{
		QVector3D p;
		for (int i = 0; i <= n; i++)
		{
			p += bernsteinPolynomial(t, n, i) * controlPoints[i];
		}
		return p;
	}
}
