#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <QThread>
#include <QImage>
#include <QMutex>
#include <QWaitCondition>
#include "converserec.h"

class RenderThread : public QThread
{
    Q_OBJECT
public:
    explicit RenderThread(QObject *parent = 0);
    void render(const QImage &image, qreal gamma, const ConverseRec *rec, int zoom, UnboxingPolicy policy);
    ~RenderThread();

protected:
    void run();

signals:
    void renderedImage(const QImage &image);

private:
    QImage rawImage;
    QMutex mutex;
    QWaitCondition condition;
    qreal gamma;
    int zoom;
    UnboxingPolicy policy;
    const ConverseRec *converseRec;
    bool abort;
    bool restart;
    
};

#endif // RENDERTHREAD_H
