#ifndef PLOYGONTRANSLATION_H
#define PLOYGONTRANSLATION_H

#include <QPointF>
#include <QRectF>
#include "utils.h"

class Translator
{
public:
	Translator();
	Translator(const Utils::Quadrangle& quad, const QRectF& mediateRect);
	QPointF translate(const QPointF& p);
	bool isValid();

private:
	QRectF mediateRect;
	qreal a[3];
	qreal b[3];
	qreal d[3];
};

#endif // PLOYGONTRANSLATION_H
