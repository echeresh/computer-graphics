#ifndef QUADRANGLEMAPPER_H
#define QUADRANGLEMAPPER_H

#include <QPoint>
#include <QImage>
#include "utils.h"
#include "mipmap.h"
#include "translator.h"

class QuadrangleMapper
{
public:
	QuadrangleMapper();
	QuadrangleMapper(const Utils::Quadrangle& quad, const QRectF& mediateRect);
	QPointF translate(const QPointF& p);
	bool isValid();
	void draw(const MipMap& mipMap, QImage& buffer);

private:
	Translator translator;
	Utils::Quadrangle quadrangle;
};

class LineDrawer
{
public:
	LineDrawer(const QPoint& p0, const QPoint& p1);
	bool next(QPoint &nextPoint);

private:
	int dx;
	int dy;
	int err;
	QPoint p;
	QPoint ip;
	QPoint sp;
	QPoint p1;
};
#endif // QUADRANGLEMAPPER_H
