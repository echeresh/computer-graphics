#include <qmath.h>
#include <QColor>
#include <QVector>
#include <QLineF>
#include <QSize>
#include "utils.h"

namespace Utils
{
	qreal QuadrangleF::maxSide() const
	{
		qreal maxDist = qMax(QLineF(p0, p1).length(), QLineF(p1, p3).length());
		maxDist = qMax(maxDist, QLineF(p0, p2).length());
		maxDist = qMax(maxDist, QLineF(p2, p3).length());
		return maxDist;
	}

	QuadrangleF QuadrangleF::scaleTo(const QSize& size) const
	{
		QuadrangleF quad =
		{
			QPointF(size.width() * p0.x(), size.height() * p0.y()),
			QPointF(size.width() * p1.x(), size.height() * p1.y()),
			QPointF(size.width() * p2.x(), size.height() * p2.y()),
			QPointF(size.width() * p3.x(), size.height() * p3.y())
		};
		return quad;
	}

	Quadrangle::Quadrangle()
	{
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
		b(qBlue(rgb))
	{
	}

	RgbF::RgbF(qreal r, qreal g, qreal b) :
		r(r),
		g(g),
		b(b)
	{
	}

	QRgb RgbF::toRgb() const
	{
		return qRgb(qRound(r),
					qRound(g),
					qRound(b));
	}

	RgbF bilinearFiltering(qreal ratio, const RgbF& rgb0, const RgbF& rgb1)
	{
		return RgbF((1 - ratio) * rgb0.r + ratio * rgb1.r,
					(1 - ratio) * rgb0.g + ratio * rgb1.g,
					(1 - ratio) * rgb0.b + ratio * rgb1.b);
	}

	QVector<qreal> solveSLE(const QVector<QVector<qreal> >& sle)
	{
		QVector<QVector<qreal> > A(sle);
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

	//mirror point for square [0;1]x[0;1]
	QPointF mirrorPoint(const QPointF& point)
	{
		QPointF resPoint(point);
		while (resPoint.x() < 0)
		{
			resPoint.rx() += 2;
		}
		while (resPoint.y() < 0)
		{
			resPoint.ry() += 2;
		}
		while (resPoint.x() >= 2)
		{
			resPoint.rx() -= 2;
		}
		while (resPoint.y() >= 2)
		{
			resPoint.ry() -= 2;
		}
		if (resPoint.x() >= 1)
		{
			resPoint.rx() = 2 - resPoint.x();
		}
		if (resPoint.y() >= 1)
		{
			resPoint.ry() = 2 - resPoint.y();
		}
		return resPoint;
	}
}
