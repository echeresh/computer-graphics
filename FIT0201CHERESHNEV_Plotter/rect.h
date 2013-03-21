#ifndef RECT_H
#define RECT_H
#include <QtGlobal>

class Rect
{
public:
	Rect();
	Rect(qreal _x, qreal _y, qreal w, qreal h);
	qreal left() const;
	qreal right() const;
	qreal top() const;
	qreal bottom() const;
	qreal x() const;
	qreal y() const;
	qreal width() const;
	qreal height() const;
private:
	qreal _x;
	qreal _y;
	qreal w;
	qreal h;
};

#endif // RECT_H
