#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QImage>
#include <QVector>
#include <QPair>
#include <QVector3D>
#include "beziersurface.h"
#include "utils.h"

class Widget : public QWidget, public Projector
{
	Q_OBJECT
public:
	explicit Widget(QWidget *parent = 0);
	void paintEvent(QPaintEvent *);
	void resizeEvent(QResizeEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void wheelEvent(QWheelEvent *);
	QPoint to2D(const QVector3D& v);

private:
	float R;
	qreal scale;
	QVector<Utils::Line3D> lines;
	QVector3D eye;
	QVector3D up;

	QVector3D aabb; //point of Axis-Aligned Bounding Box with max x,y,z

	bool leftPressed;
	QPoint lastPoint;

	BezierSurface bezierSurface;
	QVector<BezierGrid> grids;
	int nSegments;

	QPoint getProjection(const QVector3D& v);
	QPoint getIntersection(const QVector3D& v0, const QVector3D& v1);
	QVector3D calcBoundingBox();
	QVector<Utils::Line3D> createBox(const QVector3D& v);
	QVector3D camera();
	void calcGrid(int nSegments);
	void drawAxis(const QPoint &center, const QPoint &dirVector, QRgb rgb);
	void updatePlot();

signals:
	void segmentsChanged(int nSegments);

private slots:
	void updateSegments(int nSegments);
	void openFile();
	
};

#endif // WIDGET_H
