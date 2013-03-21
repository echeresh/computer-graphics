#include "rect.h"

Rect::Rect()
{
	this->_x = 0;
	this->_y = 0;
	this->w = 0;
	this->h = 0;
}

Rect::Rect(qreal x, qreal y, qreal w, qreal h)
{
	this->_x = x;
	this->_y = y;
	this->w = w;
	this->h = h;
}

qreal Rect::left() const
{
	return _x;
}
qreal Rect::right() const
{
	return _x + w;
}
qreal Rect::top() const
{
	return _y + h;
}
qreal Rect::bottom() const
{
	return _y;
}
qreal Rect::x() const
{
	return _x;
}

qreal Rect::y() const
{
	return _y;
}

qreal Rect::width() const
{
	return w;
}

qreal Rect::height() const
{
	return h;
}
