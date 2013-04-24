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
	mipFiltering(mipFiltering),
	viewMode(Default)
{
	QImage textureImage(texturePath);
	if (textureImage.isNull())
	{
		return;
	}
	texture = QImage(textureImage.width() * 3 / 2, textureImage.height(), QImage::Format_RGB32);
	texture.fill(Qt::white);
	QPainter painter;
	painter.begin(&texture);
	painter.drawImage(0, 0, textureImage);
	painter.end();

	QPoint prevLevelPoint;
	QPoint levelPoint(textureImage.width(), 0);
	levels.push_back(Level(QPoint(), textureImage.size()));
	QSize textureSize(textureImage.size().width() / 2, textureImage.size().height() / 2);
	while(!textureSize.isEmpty())
	{
		levels.push_back(Level(levelPoint, textureSize));
		fillLevel(levelPoint, textureSize, prevLevelPoint);
		prevLevelPoint = levelPoint;
		levelPoint.ry() += textureSize.height();
		textureSize.rwidth() /= 2;
		textureSize.rheight() /= 2;
	}
	maxLevel = levels.size() - 1;
}

void MipMap::setViewMode(ViewMode viewMode)
{
	this->viewMode = viewMode;
}

void MipMap::setMaxLevel(int level)
{
	maxLevel = qMin(level, levels.size() - 1);
	maxLevel = qMax(maxLevel, 0);
}

QRgb MipMap::getFourAverage(const QPoint& p)
{
	QRgb p0 = texture.pixel(p.x(), p.y());
	QRgb p1 = texture.pixel(p.x(), p.y() + 1);
	QRgb p2 = texture.pixel(p.x() + 1, p.y());
	QRgb p3 = texture.pixel(p.x() + 1, p.y() + 1);
	return qRgb(qRound((qRed(p0) + qRed(p1) + qRed(p2) + qRed(p3)) / 4.),
				qRound((qGreen(p0) + qGreen(p1) + qGreen(p2) + qGreen(p3)) / 4.),
				qRound((qBlue(p0) + qBlue(p1) + qBlue(p2) + qBlue(p3)) / 4.));
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
	QPointF pixPoint = QPointF(levels[nLevel].size.width() * pixel.x() - .5,
							 levels[nLevel].size.height() * pixel.y() - .5);
	switch(layerFiltering)
	{
		case Point:
		{
			QPoint roundedPixel(qRound(pixPoint.x()), qRound(pixPoint.y()));
			return Utils::RgbF(texturePixel(roundedPixel, nLevel));
		}
		case Linear:
		{
			int xMin = qFloor(pixPoint.x());
			int yMin = qFloor(pixPoint.y());

			Utils::RgbF xTop = Utils::bilinearFiltering(pixPoint.x() - xMin,
														Utils::RgbF(texturePixel(QPoint(xMin, yMin), nLevel)),
														Utils::RgbF(texturePixel(QPoint(xMin + 1, yMin), nLevel)));
			Utils::RgbF xDown = Utils::bilinearFiltering(pixPoint.x() - xMin,
														 Utils::RgbF(texturePixel(QPoint(xMin, yMin + 1), nLevel)),
														 Utils::RgbF(texturePixel(QPoint(xMin + 1, yMin + 1), nLevel)));
			return Utils::bilinearFiltering(pixPoint.y() - yMin, xTop, xDown);
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
	if (viewMode == UV)
	{
		QPointF uvPoint = Utils::mirrorPoint(pixel);
		return qRgb(qRound(uvPoint.x() * 255), qRound(uvPoint.y() * 255), 0);
	}

	FilteringType mipType = mipFiltering;
	qreal levelF = 0.;
	if (mipType == None)
	{
		mipType = Point;
	}
	else
	{
		qreal upSide = 0.;
		int upLevel = 0;
		while ((upSide = quad.scaleTo(levels[upLevel].size).maxSide()) > 1)
		{
			upLevel++;
			if (upLevel > maxLevel)
			{
				break;
			}
		}
		if (upLevel == 0 || upLevel > maxLevel)
		{
			levelF = static_cast<qreal>(qMin(upLevel, maxLevel));
			mipType = Point;
		}
		else
		{
			int downLevel = upLevel - 1;
			qreal downSide = quad.scaleTo(levels[downLevel].size).maxSide();
			levelF = downLevel + (downSide - 1.) / (downSide - upSide);
		}
	}
	if (viewMode == MIP)
	{
		levelF = (mipType == Point) ? qRound(levelF) : levelF;
		int v = qRound(levelF * 255. / levels.size());
		return qRgb(0, 0, v);//black->blue or qRgb(v, v, 255) blue->white
	}
	switch(mipType)
	{
		case Point:
			return layerPixel(pixel, qRound(levelF)).toRgb();
		case Linear:
		{
			int upLevel = qCeil(levelF);
			int downLevel = qFloor(levelF);
			return Utils::bilinearFiltering(levelF - downLevel,
											layerPixel(pixel, downLevel),
											layerPixel(pixel, upLevel)).toRgb();
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
