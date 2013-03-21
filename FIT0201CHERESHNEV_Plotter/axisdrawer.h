#ifndef AXISDRAWER_H
#define AXISDRAWER_H
#include <QPoint>
#include "drawer.h"

template <class T>
class AxisDrawer
{
public:
	AxisDrawer(Drawer &drawer, T unitLength, int markLength) :
	drawer(drawer),
	unitLength(unitLength),
	markLength(markLength)
	{
	}

	void draw(int areaWidth, int areaHeight, const QPoint& center);

private:
	Drawer &drawer;
	const int unitLength;
	const int markLength;
};

#endif // AXISDRAWER_H
