#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QImage>
#include <QVector>
#include <QPair>
#include <QVector3D>
#include "beziersurfacemodel.h"
#include "beziergrid.h"
#include "utils.h"

class Widget : public QWidget, public Painter3D
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

private:
	QVector3D boundingBox; //point of Axis-Aligned Bounding Box with max x,y,z

	bool leftPressed;
	QPoint lastPoint;

	BezierSurfaceModel bezierModel;
	QVector<BezierGrid> grids;
	int nSegments;

	bool showFilling;
	bool showLight;
	bool showBoundingBox;
	bool showAxis;
	bool showPolygons;
	bool showFrame;

	void calcBoundingBox();
	void loadBezierSurface(const QString& filePath);
	void drawAxis(const QPoint &center, const QPoint &dirVector, QRgb rgb);
	void updatePlot();

signals:
	void segmentsChanged(int nSegments);

private slots:
	void updateSegments(int nSegments);
	void updateFilling(bool showFilling);
	void updateLight(bool showLight);
	void updateBoundingBox(bool showBoundingBox);
	void updateAxis(bool showAxis);
	void setBezierMode();
	void setPolygonMode();
	void updateFrame(bool showFrame);
	void openModelFile();
	void setPixelByPixelBezierDrawing();
	void setIterativeBezierDrawing();
	void setDeCasteljauBezierDrawing();
	
};

#endif // WIDGET_H
