#include <qmath.h>
#include "cassini.h"
#include "utils.h"

Cassini::Cassini(const QPoint& f0, const QPoint& f1, int R, Drawer& drawer, int thickness) :
	f0(f0),
	f1(f1),
	center((f0 + f1) / 2),
	R(R),
	thickness(thickness),
	drawer(drawer),
	leftPoint(calcIntersection(f1, f0)),
	rightPoint(calcIntersection(f0, f1))
{
}

void Cassini::draw()
{
	QPair<QPoint, QPoint> lPair = findTwoNearBest(leftPoint);
	QPair<QPoint, QPoint> rPair = findTwoNearBest(rightPoint);
	const PointPath l0 = {lPair.first, leftPoint, lPair.second};
	const PointPath l1 = {lPair.second, leftPoint, lPair.first};
	const PointPath r0 = {rPair.first, rightPoint, rPair.second};
	const PointPath r1 = {rPair.second, rightPoint, rPair.first};

	PointPath paths[] = {l0, l1, r0 ,r1};

	int dx = f1.x() - f0.x();
	int dy = f1.y() - f0.y();
	int distSquared = sqr(dx) + sqr(dy);
	CassiniType cassiniType = distSquared == 4 * R ?
						 Lemniscate :
						 (distSquared > 4 * R ?
							  DoubleLoop :
							  SingleLoop);
	setPoint(leftPoint);
	setPoint(rightPoint);

	if (drawLoop(paths[0]))
	{
		bool isCenterBreak1 = drawLoop(paths[1]);
		bool isCenterBreak2 = drawLoop(paths[2]);
		bool isCenterBreak3 = drawLoop(paths[3]);
		if ((isCenterBreak1 || isCenterBreak2 || isCenterBreak3) == false)
		{
			return;
		}
		switch(cassiniType)
		{
		case DoubleLoop:
			if (isCenterBreak1)
			{
				drawer.drawLine(paths[0].cur, paths[1].cur, thickness);
			}
			if (isCenterBreak2 && isCenterBreak3)
			{
				drawer.drawLine(paths[2].cur, paths[3].cur, thickness);
			}
			break;
		case SingleLoop:
		case Lemniscate:
			if ((sign((paths[0].cur.x() - f0.x()) * dy - (paths[0].cur.y() - f0.y()) * dx) ==
					sign((paths[2].cur.x() - f0.x()) * dy - (paths[2].cur.y() - f0.y()) * dx)) ^
					(cassiniType == SingleLoop))
			{
				qSwap(paths[2], paths[3]);
				qSwap(isCenterBreak2, isCenterBreak3);
			}
			if (isCenterBreak2)
			{
				drawer.drawLine(paths[0].cur, paths[2].cur, thickness);
			}
			if (isCenterBreak1 && isCenterBreak3)
			{
				drawer.drawLine(paths[1].cur, paths[3].cur, thickness);
			}
			break;
		}
	}
	else
	{
		switch (cassiniType)
		{
		case SingleLoop:
			if (drawLoop(paths[0]))
			{
				PointPath path = l1;
				if (drawLoop(path))
				{
					drawer.drawLine(path.cur, paths[0].cur, thickness);
				}
			}
			break;
		case DoubleLoop:
			if (drawLoop(paths[2]))
			{
				PointPath path = r1;
				if (drawLoop(path))
				{
					drawer.drawLine(path.cur, paths[2].cur, thickness);
				}
			}
			break;
		}
	}
}

bool Cassini::drawLoop(PointPath& path)
{
	const int CENTER_DIST = 3;
	do
	{
		setPoint(path.cur);
		QPoint nextBest = findNearBest(path);
		path.prev1 = path.prev0;
		path.prev0 = path.cur;
		path.cur = nextBest;
		if ((path.cur - center).manhattanLength() < CENTER_DIST)
		{
			return true;
		}
	}
	while (path.cur != leftPoint && path.cur != rightPoint);
	return false;
}

qlonglong Cassini::calcValue(const QPoint& point)
{
	return (sqr(1LL * point.x() - f0.x()) + sqr(point.y() - f0.y())) *
			(sqr(point.x() - f1.x()) + sqr(point.y() - f1.y())) - 1LL * R * R;
}

QPoint Cassini::calcIntersection(const QPoint& lineBegin, const QPoint& lineEnd)
{
	//search for segment with different signs on its ends
	QPoint p(lineEnd.x(), lineEnd.y());	
	int sgn = sign(calcValue(p));
	QPoint dp(lineEnd.x() - lineBegin.x(), lineEnd.y() - lineBegin.y());
	if (lineBegin == lineEnd)
	{
		dp = QPoint(1, 1);
	}
	while(sign(calcValue(p + dp)) == sgn)
	{
		p += dp;
		dp *= 2;
	}

	//binary search
	const int SEGMENT_ACCURACY = 2;
	QPoint lp(p);
	QPoint rp(p + dp);
	while ((rp - lp).manhattanLength() > SEGMENT_ACCURACY)
	{
		QPoint mp = (lp + rp) / 2;
		(sign(calcValue(mp)) * sign(calcValue(rp)) <= 0 ? lp : rp) = mp;
	}

	//find best near pixel
	p = lp;
	const int WIDTH_SEARCH = 2;
	for (QPoint dp(-WIDTH_SEARCH, -WIDTH_SEARCH); dp.x() <= WIDTH_SEARCH; dp.rx()++)
	{
		for (dp.ry() = -WIDTH_SEARCH; dp.y() <= WIDTH_SEARCH; dp.ry()++)
		{
			if (qAbs(calcValue(lp + dp)) < qAbs(calcValue(p)))
			{
				p = lp + dp;
			}
		}
	}
	return p;
}

QPoint Cassini::findNearBest(const PointPath &path)
{
	qlonglong minValue = LLONG_MAX;
	QPoint minPoint;
	//considering 8 neighbouring pixels except current, two previous and
	//adjacent(have common side) with two previous: manhattanLength <= 1
	for (QPoint dp(-1, -1); dp.x() <= 1; dp.rx()++)
	{
		for (dp.ry() = -1; dp.y() <= 1; dp.ry()++)
		{
			QPoint p = path.cur + dp;
			qlonglong value;
			if (p != path.cur &&
					(p - path.prev0).manhattanLength() > 1 &&
					(p - path.prev1).manhattanLength() > 1 &&
					(value = qAbs(calcValue(p))) < minValue)
			{
				minValue = value;
				minPoint = p;
			}
		}
	}
	return minPoint;
}

QPair<QPoint, QPoint> Cassini::findTwoNearBest(const QPoint& point)
{
	qlonglong minValue0 = LLONG_MAX;
	qlonglong minValue1 = LLONG_MAX;
	QPoint minPoint0;
	QPoint minPoint1;
	for (QPoint dp(-1, -1); dp.x() <= 1; dp.rx()++)
	{
		for (dp.ry() = -1; dp.y() <= 1; dp.ry()++)
		{
			QPoint p = point + dp;
			if (p != point)
			{
				qlonglong value =  qAbs(calcValue(p));
				if (value < minValue0)
				{
					minPoint1 = minPoint0;
					minValue1 = minValue0;
					minPoint0 = p;
					minValue0 = value;
				}
				else if (value < minValue1)
				{
					minPoint1 = p;
					minValue1 = value;
				}
			}
		}
	}
	return qMakePair(minPoint0, minPoint1);
}

void Cassini::setPoint(const QPoint& point)
{
	drawer.fillCircle(point, thickness);
}
