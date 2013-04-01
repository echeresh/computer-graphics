#ifndef CASSINI_H
#define CASSINI_H
#include <QPoint>
#include <QImage>
#include "drawer.h"

class Cassini
{
public:
	Cassini(const QPoint& f0, const QPoint& f1, int R, Drawer &drawer, int thickness);
	void draw();

private:
	const QPoint f0;
	const QPoint f1;
	const QPoint center;
	const int R;
	const int thickness;
	const QPoint leftPoint;
	const QPoint rightPoint;
	Drawer &drawer;

	//stores current pixel and 2 previous pixels
	struct PointPath
	{
		QPoint cur;
		QPoint prev0;
		QPoint prev1;
	};

private:
	qlonglong calcValue(const QPoint& point);
	QPoint calcIntersection(const QPoint& lineBegin, const QPoint& lineEnd);
	QPoint findNearBest(const PointPath& path);
	QPair<QPoint, QPoint> findTwoNearBest(const QPoint& point);
	void setPoint(const QPoint& point);
	bool drawLoop(PointPath& path);
};

enum CassiniType
{
	SingleLoop,
	Lemniscate,
    DoubleLoop
};

#endif // CASSINI_H
