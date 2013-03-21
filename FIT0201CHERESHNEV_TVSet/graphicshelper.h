#ifndef GRAPHICSHELPER_H
#define GRAPHICSHELPER_H
#include <QRgb>
#include <QtGlobal>
#include <QImage>
#include "converserec.h"

struct YUV422
{
   int y0;
   int y1;
   int u;
   int v;
   YUV422(QRgb rgb);
};

class GraphicsHelper
{
public:
    GraphicsHelper(qreal gamma, int zoom, const ConverseRec *converseRec);
    QRgb fromYUV(int y, int u, int v);
    void setZoomPixel(QImage &image, int nozoomX, int nozoomY, QRgb rgb);

private:
    const qreal Kb;
    const qreal Kr;
    const qreal gamma;
    const int zoom;
};

#endif // GRAPHICSHELPER_H
