#ifndef AXISDRAWER_H
#define AXISDRAWER_H
#include <QPoint>
#include "drawer.h"

class AxisDrawer
{
public:
    AxisDrawer(Drawer &drawer, qreal unitLength, int markLength);
	void draw(int areaWidth, int areaHeight, const QPoint& center);

private:
	Drawer &drawer;
    qreal unitLength;
	const int markLength;
};

#endif // AXISDRAWER_H
