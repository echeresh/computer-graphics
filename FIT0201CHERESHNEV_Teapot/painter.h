#ifndef PROJECTOR_H
#define PROJECTOR_H

#include <QVector3D>
#include <QPoint>
#include <QImage>
#include "utils.h"

class Painter3D
{
public:
	Painter3D();
	QPoint getProjection(const QVector3D& v, bool* isCross = NULL) const;
	Utils::Point3D projectPoint(const QVector3D& v);
	QPoint getIntersection(const QVector3D& v0, const QVector3D& v1, bool* isCross = NULL) const;
	QPoint to2D(const QVector3D& v) const;
	float distanceFromCamera(const QVector3D& v) const;

	void drawLine(const QPoint& p0, const QPoint& p1);
	void drawLine(const QVector3D &v0, const QVector3D &v1);
	void drawLine(const Utils::Line3D &line);

	void setBezierDrawing(int drawMode);
	void drawCubicBezier(const QVector3D* controlPoints);
	void drawCubicBezierPixelByPixel(const QVector3D *controlPoints);
	void drawCubicBezierRecursive(const QVector3D* controlPoints, int depth = 4);
	void drawCubicBezierIterative(const QVector3D* controlPoints, float step = .05f);
	void drawCubicBezierAdaptive(const QVector3D *controlPoints);

	void drawBox(const QVector3D& diag, const QVector3D& center = QVector3D());
	void drawTriangles(const QVector<Utils::Triangle3D> &triangles, bool showFrame, QRgb lineColor,
					   bool showFilling, QRgb fillColor, bool showLight);

	void fillTriangle(const Utils::Triangle3D &triangle);
	void fillTriangle(QPoint p0, QPoint p1, QPoint p2);

	void recalcView(const QVector3D& boundingBox);
	void xRotate(float angle);
	void yRotate(float angle);
	void zoom(int dr);

	void resizeBuffer(const QSize &newSize);
	void setPixel(const QPoint& p);
	void setColor(QRgb newRgb);

	static const int STEP_BY_STEP_BEZIER_DRAWING = 0;
	static const int ITERATIVE_BEZIER_DRAWING = 1;
	static const int DE_CASTELJAU_BEZIER_DRAWING = 2;

protected:
	QImage buffer;
	QRgb rgb;

private:
	float R;
	float scale;
	QVector3D eye;
	QVector3D up;
	int bezierDrawMode;
};

//Bresenham's line algorithm with states
//Needed for triangle filling
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

#endif // PROJECTOR_H
