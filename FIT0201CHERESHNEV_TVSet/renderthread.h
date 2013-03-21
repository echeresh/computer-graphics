#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <QThread>
#include <QImage>
#include <QMutex>
#include <QWaitCondition>
#include "converserec.h"

struct RenderTask
{
    qreal gamma;
    int zoom;
    UnboxingPolicy unboxingPolicy;
    const ConverseRec *converseRec;
    const QImage *srcImage;
};

class RenderThread : public QThread
{
    Q_OBJECT
public:
    explicit RenderThread(QObject *parent = 0);
    void render(const QImage *srcImage, qreal gamma, const ConverseRec *rec, int zoom, UnboxingPolicy policy);
    ~RenderThread();

protected:
    void run();

signals:
    void renderedImage(const QImage &image);

private:
    QMutex mutex;
    QWaitCondition condition;
    RenderTask renderTask;
    bool abort;
    bool restart;
    
};
#endif // RENDERTHREAD_H
