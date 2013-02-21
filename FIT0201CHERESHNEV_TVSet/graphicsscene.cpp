#include <QUrl>
#include <QList>
#include "graphicsscene.h"

void GraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}

void GraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}

void GraphicsScene::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    if (event->mimeData()->urls().isEmpty())
    {
        return;
    }
    QUrl url = event->mimeData()->urls().first();
    QImage image(url.toLocalFile());
    if (image.isNull())
    {
        return;
    }
    emit loadedImage(image);
}
