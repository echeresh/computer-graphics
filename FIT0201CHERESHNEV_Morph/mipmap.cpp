#include <QPainter>
#include <QSize>
#include <qmath.h>
#include "mipmap.h"

Level::Level()
{
}

Level::Level(const QPoint& startPoint, const QSize& size) :
	startPoint(startPoint),
	size(size)
{
}

MipMap::MipMap(const QString& texturePath, FilteringType layerFiltering, FilteringType mipFiltering) :
	layerFiltering(layerFiltering),
	mipFiltering(mipFiltering)
{
	QImage textureImage(texturePath);
	if (textureImage.isNull())
		return;
	texture = QImage(textureImage.width() * 3 / 2, textureImage.height(), QImage::Format_ARGB32);

	QPainter painter;
	painter.begin(&texture);
	painter.drawImage(0, 0, textureImage);
	painter.end();

	QPoint prevLevelPoint;
	QPoint levelPoint(textureImage.width(), 0);
	levels.push_front(Level(QPoint(), textureImage.size()));
	for (QSize textureSize = textureImage.size() / 2; !textureSize.isEmpty(); textureSize.rwidth() /= 2, textureSize.rheight() /= 2)
	{
		levels.push_front(Level(levelPoint, textureSize));
		fillLevel(levelPoint, textureSize, prevLevelPoint);
		prevLevelPoint = levelPoint;
		levelPoint.ry() += textureSize.height();
	}
}

QRgb MipMap::getFourAverage(const QPoint& p)
{
	QRgb p0 = texture.pixel(p.x(), p.y());
	QRgb p1 = texture.pixel(p.x(), p.y() + 1);
	QRgb p2 = texture.pixel(p.x() + 1, p.y());
	QRgb p3 = texture.pixel(p.x() + 1, p.y() + 1);
	return qRgba(static_cast<int>((qRed(p0) + qRed(p1) + qRed(p2) + qRed(p3)) / 4.),
				static_cast<int>((qGreen(p1) + qGreen(p1) + qGreen(p2) + qGreen(p3)) / 4.),
				static_cast<int>((qBlue(p2) + qBlue(p1) + qBlue(p2) + qBlue(p3)) / 4.),
				static_cast<int>((qAlpha(p0) + qAlpha(p1) + qAlpha(p2) + qAlpha(p3)) / 4.));
}

void MipMap::fillLevel(const QPoint& levelPoint, const QSize& size, const QPoint& prevLevelPoint)
{
	for (int i = 0; i < size.width(); i++)
	{
		for (int j = 0; j < size.height(); j++)
		{
			texture.setPixel(i + levelPoint.x(), j + levelPoint.y(),
							 getFourAverage(prevLevelPoint + QPoint(2 * i, 2 * j)));
		}
	}
}

Utils::RgbF MipMap::layerPixel(const QPointF& pixel, int nLevel) const
{
	switch(layerFiltering)
	{
		case Point:
		{
			QPoint pixPoint = QPoint(qRound(levels[nLevel].size.width() * pixel.x() - .5),
									 qRound(levels[nLevel].size.height() * pixel.y() - .5));
			return Utils::RgbF(texturePixel(pixPoint, nLevel));
		}
		case Linear:
		{
			qreal x = levels[nLevel].size.width() * pixel.x() - .5;
			qreal y = levels[nLevel].size.height() * pixel.y() - .5;
			int xMin = qFloor(x);
			int yMin = qFloor(y);

			Utils::RgbF xTop = Utils::bilinearFiltering(x - xMin,
														Utils::RgbF(texturePixel(QPoint(xMin, yMin), nLevel)),
														Utils::RgbF(texturePixel(QPoint(xMin + 1, yMin), nLevel)));
			Utils::RgbF xDown = Utils::bilinearFiltering(x - xMin,
														 Utils::RgbF(texturePixel(QPoint(xMin, yMin + 1), nLevel)),
														 Utils::RgbF(texturePixel(QPoint(xMin + 1, yMin + 1), nLevel)));
			return Utils::bilinearFiltering(y - yMin, xTop, xDown);
		}
		default:
			return Utils::RgbF(qRgb(0, 0, 0));
	}
}

QRgb MipMap::texturePixel(const QPoint& pixel, int nLevel) const
{
	const Level& level = levels[nLevel];
	int doubleWidth = 2 * level.size.width();
	int doubleHeight = 2 * level.size.height();
	QPoint mirrorPixel((pixel.x() % doubleWidth + doubleWidth) % doubleWidth,
					   (pixel.y() % doubleHeight + doubleHeight) % doubleHeight);
	if (mirrorPixel.x() >= level.size.width())
	{
		mirrorPixel.setX(doubleWidth - mirrorPixel.x() - 1);
	}
	if (mirrorPixel.y() >= level.size.height())
	{
		mirrorPixel.setY(doubleHeight - mirrorPixel.y() - 1);
	}
	return texture.pixel(level.startPoint + mirrorPixel);
}

QRgb MipMap::map(const QPointF& pixel, const Utils::QuadrangleF& quad) const
{
	qreal maxSide = quad.maxSide();
	qreal levelF = qLn(1 / maxSide) / qLn(2.);
	if (levelF <= 0)
	{
		return layerPixel(pixel, 0).toRgb();
	}
	if (levelF >= levels.size() - 1)
	{
		return layerPixel(pixel, levels.size() - 1).toRgb();
	}
	switch(mipFiltering)
	{
		case None:
			return layerPixel(pixel, levels.size() - 1).toRgb();
		case Point:
			return layerPixel(pixel, qRound(levelF)).toRgb();
		case Linear:
		{
			int l0 = qFloor(levelF);
			int l1 = l0 + 1;

			qreal side0 = maxSide * levels[l0].size.width();
			qreal side1 = maxSide * levels[l1].size.width();

			return Utils::bilinearFiltering((side1 - 1.) / (side1 - side0),
											layerPixel(pixel, l0),
											layerPixel(pixel, l1)).toRgb();
		}
		default:
			return qRgb(0, 0, 0);
	}
}

void MipMap::setLayerFiltering(FilteringType filtering)
{
	layerFiltering = filtering;
}

void MipMap::setMipFiltering(FilteringType filtering)
{
	mipFiltering = filtering;
}

FilteringType MipMap::getLayerFiltering()
{
	return layerFiltering;
}

FilteringType MipMap::getMipFiltering()
{
	return mipFiltering;
}
