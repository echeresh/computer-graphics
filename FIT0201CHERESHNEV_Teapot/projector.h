#ifndef PROJECTOR_H
#define PROJECTOR_H
#include <QVector3D>
#include <QPoint>
#include <QImage>

class Projector
{
public:
	virtual QPoint getProjection(const QVector3D& v) = 0;
	virtual QPoint getIntersection(const QVector3D& v0, const QVector3D& v1) = 0;
	void drawLine(const QPoint& p0, const QPoint& p1);
	void drawLine(const QVector3D& v0, const QVector3D& v1);
	void resizeBuffer(const QSize &newSize);
	void setPixel(const QPoint& p);
	void setColor(QRgb newRgb);
protected:
	QImage buffer;
	QRgb rgb;
};

#endif // PROJECTOR_H
