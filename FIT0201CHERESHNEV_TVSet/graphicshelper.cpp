#include <QtCore/qmath.h>
#include <QColor>
#include "graphicshelper.h"

YUV422::YUV422(QRgb rgb) :
    y0(qAlpha(rgb)),
    y1(qGreen(rgb)),
    u(qRed(rgb)),
    v(qBlue(rgb))
{
}

GraphicsHelper::GraphicsHelper(qreal gamma, int zoom, const ConverseRec *converseRec) :
    Kb(converseRec->Kb),
    Kr(converseRec->Kr),
    gamma(gamma),
    zoom(zoom)
{
}

QRgb GraphicsHelper::fromYUV(int y, int u, int v)
{
    qreal u0 = (u - uvMin) / static_cast<qreal>(uvMax - uvMin) - .5;  //[16, 235]->[-0.5, 0.5]
    qreal v0 = (v - uvMin) / static_cast<qreal>(uvMax - uvMin) - .5;  //[16, 235]->[-0.5, 0.5]
    qreal y0 = (y - yMin) / static_cast<qreal>(yMax - yMin);          //[16, 240]->[0, 1]
    y0 = qPow(y0, gamma); //gamma-correction

    int r = static_cast<int>(255 * (y0 - 2 * Kr * v0 + 2 * v0) + .5);
    int g = static_cast<int>(255 * (y0 - 2 * (Kr * v0 * (1 - Kr) + Kb * u0 * (1 - Kb)) / (1 - Kb - Kr)) + .5);
    int b = static_cast<int>(255 * (y0 - 2 * Kb * u0 + 2 * u0) + .5);
    r = qMin(255, qMax(0, r));
    g = qMin(255, qMax(0, g));
    b = qMin(255, qMax(0, b));
    return QColor(r, g, b).rgb();
}

void GraphicsHelper::setZoomPixel(QImage &image, int nozoomX, int nozoomY, QRgb rgb)
{
    for (int k = 0; k < zoom; k++)
    {
        for (int l = 0; l < zoom; l++)
        {
            image.setPixel(nozoomX * zoom + k, nozoomY * zoom + l, rgb);
        }
    }
}
