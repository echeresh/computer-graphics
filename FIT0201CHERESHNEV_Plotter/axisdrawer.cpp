#include "axisdrawer.h"

AxisDrawer::AxisDrawer(Drawer &drawer, qreal unitLength, int markLength) :
	drawer(drawer),
	unitLength(unitLength),
	markLength(markLength)
{
}

void AxisDrawer::draw(int areaWidth, int areaHeight, const QPoint& center)
{
	//drawing y-Axis
	if (center.x() < areaWidth && center.x() >= 0)
	{
		drawer.drawLine(QPoint(center.x(), 0), QPoint(center.x(), areaHeight));
		for (qreal y = static_cast<qreal>(center.y()); y < areaHeight; y += unitLength)
		{
			int iy = static_cast<int>(y + .5);
			drawer.drawLine(QPoint(center.x() - markLength, iy), QPoint(center.x() + markLength, iy));
		}
		for (qreal y = static_cast<qreal>(center.y()); y >= 0; y -= unitLength)
		{
			int iy = static_cast<int>(y + .5);
			drawer.drawLine(QPoint(center.x() - markLength, iy), QPoint(center.x() + markLength, iy));
		}
	}
	//drawing x-Axis
	if (center.y() < areaHeight && center.y() >= 0)
	{
		drawer.drawLine(QPoint(0, center.y()), QPoint(areaWidth, center.y()));
		for (qreal x = static_cast<qreal>(center.x()); x < areaWidth; x += unitLength)
		{
			int ix = static_cast<int>(x + .5);
			drawer.drawLine(QPoint(ix, center.y() - markLength), QPoint(ix, center.y() + markLength));
		}
		for (qreal x = static_cast<qreal>(center.x()); x >= 0; x -= unitLength)
		{
			int ix = static_cast<int>(x + .5);
			drawer.drawLine(QPoint(ix, center.y() - markLength), QPoint(ix, center.y() + markLength));
		}
	}
}
