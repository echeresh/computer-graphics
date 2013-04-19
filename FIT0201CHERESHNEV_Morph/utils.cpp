#include <qmath.h>
#include <QColor>
#include <QVector>
#include <QLineF>
#include "utils.h"

namespace Utils
{
	qreal QuadrangleF::maxSide() const
	{
		qreal maxDist = qMax(QLineF(p0, p1).length(), QLineF(p1, p2).length());
		maxDist = qMax(maxDist, QLineF(p2, p3).length());
		maxDist = qMax(maxDist, QLineF(p3, p0).length());
		return maxDist;
	}

	Quadrangle::Quadrangle(const QPoint& p0, const QPoint& p1, const QPoint& p2, const QPoint& p3) :
		p0(p0),
		p1(p1),
		p2(p2),
		p3(p3)
	{
	}

	RgbF::RgbF(QRgb rgb) :
		r(qRed(rgb)),
		g(qGreen(rgb)),
		b(qBlue(rgb)),
		a(qAlpha(rgb))
	{
	}

	RgbF::RgbF(qreal r, qreal g, qreal b, qreal a) :
		r(r),
		g(g),
		b(b),
		a(a)
	{
	}

	QRgb RgbF::toRgb() const
	{
		return qRgba(qRound(r),
					 qRound(g),
					 qRound(b),
					 qRound(a));
	}

	RgbF bilinearFiltering(qreal ratio, const RgbF& rgb0, const RgbF& rgb1)
	{
		return RgbF((1 - ratio) * rgb0.r + ratio * rgb1.r,
					(1 - ratio) * rgb0.g + ratio * rgb1.g,
					(1 - ratio) * rgb0.b + ratio * rgb1.b,
					(1 - ratio) * rgb0.a + ratio * rgb1.a);
	}

	QVector<double> solveSLE(QVector<QVector<qreal> >& A)
	{
		const int N = A.size();
		for (int nCol = 0; nCol < N; nCol++)
		{
			int curCol = nCol;
			for (int i = nCol; i< N; ++i)
			{
				if (qAbs(A[i][nCol]) > qAbs(A[curCol][nCol]))
				{
					curCol = i;
				}
			}
			for (int i = nCol; i <= N; i++)
			{
				qSwap(A[curCol][i], A[nCol][i]);
			}

			qreal d = A[nCol][nCol];
			for (int i = nCol; i <= N; i++)
			{
				A[nCol][i] /= d;
			}

			for (int i = 0; i < N; i++)
			{
				if (i != nCol)
				{
					double d = A[i][nCol];
					for (int j = nCol; j <= N; ++j)
					{
						A[i][j] -= A[nCol][j] * d;
					}
				}
			}
		}
		QVector<qreal> x(N);
		for (int i = 0; i< N; ++i)
		{
			x[i] = A[i][N];
		}
		return x;
	}
}
