#ifndef GRAPHICSSCENETV_H
#define GRAPHICSSCENETV_H
#include <QGraphicsScene>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QDebug>

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    void dragEnterEvent(QGraphicsSceneDragDropEvent* event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent* event);
    void dropEvent(QGraphicsSceneDragDropEvent* event);

signals:
    void loadedImage(const QImage &image);
};

#endif // GRAPHICSSCENETV_H
