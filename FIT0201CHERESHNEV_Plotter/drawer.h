#ifndef DRAWER_H
#define DRAWER_H
#include <QPoint>
#include <QColor>

class Drawer
{
public:
	Drawer();
	QRgb setColor(QRgb color);
	bool setPixel(const QPoint& p);
	virtual bool setPixel(const QPoint& p, QRgb color) = 0;
	void drawLine(const QPoint& p0, const QPoint& p1);
	void drawLine(const QPoint& p0, const QPoint& p1, int thickness);
	void drawRect(const QPoint& p0, const QPoint& p1);
	void fillRect(const QPoint& p0, const QPoint& p1);
	void fillCircle(const QPoint& center, int diameter);

protected:
	QRgb color;
};

#endif // DRAWER_H
