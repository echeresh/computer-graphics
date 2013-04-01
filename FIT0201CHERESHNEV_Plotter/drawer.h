#ifndef DRAWER_H
#define DRAWER_H
#include <QPoint>
#include <QColor>
#include <QImage>

class Drawer
{
public:
	Drawer();
	QRgb setColor(QRgb color);
	void setPixel(const QPoint& p);
    void setPixelXor(const QPoint& p, QRgb rgb);
	void drawLine(const QPoint& p0, const QPoint& p1, int thickness = 1);
	void drawRect(const QPoint& p0, const QPoint& p1);
	void fillRect(const QPoint& p0, const QPoint& p1);
	void fillCircle(const QPoint& center, int diameter);
    void resizeBuffer(const QSize& newSize);
    bool contains(const QPoint& p);

    virtual void setPixel(const QPoint& p, QRgb rgb);
    virtual ~Drawer();

protected:
	QRgb color;
    QImage buffer;
};

#endif // DRAWER_H
