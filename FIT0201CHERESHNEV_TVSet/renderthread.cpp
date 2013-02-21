#include <QColor>
#include <QDebug>
#include <QtCore/qmath.h>
#include "renderthread.h"
#include "graphicshelper.h"

RenderThread::RenderThread(QObject *parent) :
    QThread(parent)
{
    restart = false;
    abort = false;
}

RenderThread::~RenderThread()
{
    mutex.lock();
    abort = true;
    condition.wakeOne();
    mutex.unlock();
    wait(); //for returning from run()
}

void RenderThread::render(const QImage &image, qreal gamma, const ConverseRec *converseRec, int zoom, UnboxingPolicy policy)
{
    QMutexLocker locker(&mutex);
    this->rawImage = image;
    this->gamma = gamma;
    this->converseRec = converseRec;
    this->zoom = zoom;
    this->policy = policy;
    if (isRunning() == false)
    {
        start();
    }
    else
    {
        restart = true;
        condition.wakeOne();
    }
}

void RenderThread::run()
{
    for(;;)
    {
        mutex.lock();
        const QImage rawImageLocal = rawImage;
        GraphicsHelper helper(gamma, zoom, converseRec);
        int resWidth = rawImageLocal.width() * 2 * zoom;
        int resHeight = rawImageLocal.height() * zoom;
        bool bDub = policy == Dub;
        mutex.unlock();

        QImage res(resWidth, resHeight, QImage::Format_RGB888);
        int rawImageLocalHeight = rawImageLocal.height();
        int rawImageLocalWidth = rawImageLocal.width();
        for (int i = 0; i < rawImageLocalHeight; i++)
        {
            //first macropixel in row has not predecessor => uv-components are just copied for 2 pixels
            //for next macropixels uv-components of first pixel are copied from current macropixel and
            //uv-components of second pixel are calculated as average of current macropixel and previous macropixel
            //if unboxingPolicy == Dub then uv-components of pixels are just copied from macropixel - check in line 78
            QRgb rgb = rawImageLocal.pixel(0, i);
            YUV422 yuv(rgb);
            QRgb color1 = helper.fromYUV(yuv.y0, yuv.u, yuv.v);
            QRgb color2 = helper.fromYUV(yuv.y1, yuv.u, yuv.v);
            helper.setZoomPixel(res, 0, i, color1);
            helper.setZoomPixel(res, 1, i, color2);
            for (int j = 1; j < rawImageLocalWidth; j++)
            {
                int prevU = yuv.u;
                int prevV = yuv.v;

                rgb = rawImageLocal.pixel(j, i);
                yuv = YUV422(rgb);
                color2 = helper.fromYUV(yuv.y1, yuv.u, yuv.v);
                helper.setZoomPixel(res, j * 2 + 1, i, color2);
                if (bDub)
                {
                    color1 = helper.fromYUV(yuv.y0, yuv.u, yuv.v);
                    helper.setZoomPixel(res, j * 2, i, color1);
                }
                else
                {
                    color1 = helper.fromYUV(yuv.y0, (yuv.u + prevU)/2, (yuv.v + prevV)/2);
                    helper.setZoomPixel(res, j * 2, i, color1);
                }
            }
        }
        if (abort)
        {
            return;
        }
        emit renderedImage(res);

        mutex.lock();
        if (restart == false)
        {
            condition.wait(&mutex); //wait for next render task
        }
        restart = false;
        mutex.unlock();
    }
}
