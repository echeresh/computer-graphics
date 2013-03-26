#include "axisdrawer.h"

template <>
void AxisDrawer<int>::draw(int areaWidth, int areaHeight, const QPoint& center)
{
	//drawing y-Axis
	if (center.x() < areaWidth && center.x() >= 0)
	{
		drawer.drawLine(QPoint(center.x(), 0), QPoint(center.x(), areaHeight));
		for (int y = center.y() - unitLength * (center.y() / unitLength + 1); y < areaHeight; y += unitLength)
		{
			drawer.drawLine(QPoint(center.x() - markLength, y), QPoint(center.x() + markLength, y));
		}
	}
	//drawing x-Axis
	if (center.y() < areaHeight && center.y() >= 0)
	{
		drawer.drawLine(QPoint(0, center.y()), QPoint(areaWidth, center.y()));
		for (int x = center.x() - unitLength * (center.x() / unitLength + 1) ; x < areaWidth; x += unitLength)
		{
			drawer.drawLine(QPoint(x, center.y() - markLength), QPoint(x, center.y() + markLength));
		}
	}
}

template <>
void AxisDrawer<qreal>::draw(int areaWidth, int areaHeight, const QPoint& center)
{
	//drawing y-Axis
	if (center.x() < areaWidth && center.x() >= 0)
	{
		drawer.drawLine(QPoint(center.x(), 0), QPoint(center.x(), areaHeight));
		for (int y = center.y(); y < areaHeight; y += unitLength)
		{
			drawer.drawLine(QPoint(center.x() - markLength, y), QPoint(center.x() + markLength, y));
		}
		for (int y = center.y(); y >= 0; y -= unitLength)
		{
			drawer.drawLine(QPoint(center.x() - markLength, y), QPoint(center.x() + markLength, y));
		}
	}
	//drawing x-Axis
	if (center.y() < areaHeight && center.y() >= 0)
	{
		drawer.drawLine(QPoint(0, center.y()), QPoint(areaWidth, center.y()));
		for (int x = center.x(); x < areaWidth; x += unitLength)
		{
			drawer.drawLine(QPoint(x, center.y() - markLength), QPoint(x, center.y() + markLength));
		}
		for (int x = center.x(); x >= 0; x -= unitLength)
		{
			drawer.drawLine(QPoint(x, center.y() - markLength), QPoint(x, center.y() + markLength));
		}
	}
}
