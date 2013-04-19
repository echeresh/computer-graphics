#include <QPair>
#include <QVector>
#include "translator.h"

Translator::Translator()
{
}

Translator::Translator(const Utils::Quadrangle& quad, const QRectF& mediateRect) :
	mediateRect(mediateRect)
{
	Q_ASSERT(quad.p0.y() == quad.p1.y() && quad.p2.y() == quad.p3.y());
	const int NVARS = 8;
	const int POINT_NUMBER = 4;
	QPair<QPoint, QPointF> points[POINT_NUMBER] =
	{
		qMakePair(quad.p0, QPointF(0., 0.)),
		qMakePair(quad.p1, QPointF(1., 0.)),
		qMakePair(quad.p2, QPointF(0., 1.)),
		qMakePair(quad.p3, QPointF(1., 1.))
	};
	QVector<QVector<qreal> > A(NVARS);
	for (int i = 0; i < NVARS; i++)
	{
		A[i].resize(NVARS + 1);
	}

	//suppose d3 = 1: 8 equations, 9 variables
	const int A1_INDEX = 0;
	const int A2_INDEX = 1;
	const int A3_INDEX = 2;
	const int B1_INDEX = 3;
	const int B2_INDEX = 4;
	const int B3_INDEX = 5;
	const int D1_INDEX = 6;
	const int D2_INDEX = 7;
	const int CONST_INDEX = 8;

	int nRow = 0;
	for (int i = 0; i < POINT_NUMBER; i++)
	{
		qreal x = static_cast<qreal>(points[i].first.x());
		qreal y = static_cast<qreal>(points[i].first.y());
		qreal u = points[i].second.x();
		qreal v = points[i].second.y();

		A[nRow][A1_INDEX] = x;
		A[nRow][A2_INDEX] = y;
		A[nRow][A3_INDEX] = 1.;
		A[nRow][D1_INDEX] = -x * u;
		A[nRow][D2_INDEX] = -y * u;
		A[nRow][CONST_INDEX] = u;
		nRow++;

		A[nRow][B1_INDEX] = x;
		A[nRow][B2_INDEX] = y;
		A[nRow][B3_INDEX] = 1.;
		A[nRow][D1_INDEX] = -x * v;
		A[nRow][D2_INDEX] = -y * v;
		A[nRow][CONST_INDEX] = v;
		nRow++;
	}
	QVector<qreal> x = Utils::solveSLE(A);
	x.resize(NVARS + 1);
	x[NVARS] = 1.;

	for (int i = 0; i < 3; i++)
	{
		a[i] = x[A1_INDEX + i];
		this->b[i] = x[B1_INDEX + i];
		d[i] = x[D1_INDEX + i];
	}
}

QPointF Translator::translate(const QPointF& p)
{
	qreal divisor = d[0] * p.x() + d[1] * p.y() + d[2];
	return mediateRect.topLeft() +
			QPointF((mediateRect.width() * a[0] * p.x() + a[1] * p.y() + a[2]) / divisor,
					mediateRect.height() * (b[0] * p.x() + b[1] * p.y() + b[2]) / divisor);
}

bool Translator::isValid()
{
	for (int i = 0; i < 3; i++)
	{
		if (a[i] != a[i] || b[i] != b[i] || d[i] != d[i])
		{
			return false;
		}
	}
	return true;
}
