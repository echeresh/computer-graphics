#ifndef MIPMAP_H
#define MIPMAP_H

#include <QString>
#include <QImage>
#include "utils.h"

enum FilteringType
{
	None,
	Point,
	Linear
};

struct Level
{
	QPoint startPoint;
	QSize size;

	Level();
	Level(const QPoint& startPoint, const QSize& size);
};

class MipMap
{
public:
	MipMap(const QString& texturePath, FilteringType layerFiltering, FilteringType mipFiltering);
	QRgb map(const QPointF& pixel, const Utils::QuadrangleF& quad) const;
	void setLayerFiltering(FilteringType filtering);
	void setMipFiltering(FilteringType filtering);
	FilteringType getLayerFiltering();
	FilteringType getMipFiltering();

private:
	QImage texture;
	FilteringType layerFiltering;
	FilteringType mipFiltering;
	QList<Level> levels;

	void fillLevel(const QPoint& levelPoint, const QSize& size, const QPoint& prevLevelPoint);
	QRgb getFourAverage(const QPoint& p);
	Utils::RgbF layerPixel(const QPointF& pixel, int nLevel) const;
	QRgb texturePixel(const QPoint& pixel, int nLevel) const;
};

#endif // MIPMAP_H
