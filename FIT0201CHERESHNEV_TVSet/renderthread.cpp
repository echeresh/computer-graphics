#include <QColor>
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
    wait(); //wait for returning from run()
}

void RenderThread::render(const QImage *srcImage, qreal gamma, const ConverseRec *converseRec, int zoom, UnboxingPolicy policy)
{
    QMutexLocker locker(&mutex);

    renderTask.srcImage = srcImage;
    renderTask.gamma = gamma;
    renderTask.converseRec = converseRec;
    renderTask.zoom = zoom;
    renderTask.unboxingPolicy = policy;

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
        RenderTask localTask = renderTask;
        mutex.unlock();

        GraphicsHelper helper(localTask.gamma, localTask.zoom, localTask.converseRec);
        int destWidth = localTask.srcImage->width() * 2 * localTask.zoom;
        int destHeight = localTask.srcImage->height() * localTask.zoom;
        bool bDub = localTask.unboxingPolicy == Dub;
        const QImage *src = localTask.srcImage;
        QImage dest(destWidth, destHeight, QImage::Format_RGB888);
        int srcHeight = src->height();
        int srcWidth = src->width();
        for (int i = 0; i < srcHeight; i++)
        {
            //first macropixel in row has not predecessor => uv-components are just copied for 2 pixels
            //for next macropixels uv-components of first pixel are copied from current macropixel and
            //uv-components of second pixel are calculated as average of current macropixel and previous macropixel
            //if unboxingPolicy == Dub then uv-components of pixels are just copied from macropixel - check in line 80
            QRgb rgb = src->pixel(0, i);
            YUV422 yuv(rgb);
            QRgb color1 = helper.fromYUV(yuv.y0, yuv.u, yuv.v);
            QRgb color2 = helper.fromYUV(yuv.y1, yuv.u, yuv.v);
            helper.setZoomPixel(dest, 0, i, color1);
            helper.setZoomPixel(dest, 1, i, color2);
            for (int j = 1; j < srcWidth; j++)
            {
                int prevU = yuv.u;
                int prevV = yuv.v;

                rgb = src->pixel(j, i);
                yuv = YUV422(rgb);
                color2 = helper.fromYUV(yuv.y1, yuv.u, yuv.v);
                helper.setZoomPixel(dest, j * 2 + 1, i, color2);
                if (bDub)
                {
                    color1 = helper.fromYUV(yuv.y0, yuv.u, yuv.v);
                    helper.setZoomPixel(dest, j * 2, i, color1);
                }
                else
                {
                    color1 = helper.fromYUV(yuv.y0, (yuv.u + prevU)/2, (yuv.v + prevV)/2);
                    helper.setZoomPixel(dest, j * 2, i, color1);
                }
            }
        }
        mutex.lock();
        if (abort)
        {
            mutex.unlock();
            return;
        }
        emit renderedImage(dest);

        if (restart == false)
        {
            condition.wait(&mutex); //wait for next render task
        }
        restart = false;
        mutex.unlock();
    }
}
