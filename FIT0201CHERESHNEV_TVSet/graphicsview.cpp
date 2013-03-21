#include <QScrollBar>
#include "graphicsview.h"

ScrollBar::ScrollBar(QScrollBar *scrollBar)
{
    this->scrollBar = scrollBar;
    connect(scrollBar, SIGNAL(valueChanged(int)), SLOT(valueChanged()));
    position = -1;
}

void ScrollBar::ignoreValueChange()
{
    ignored = true;
}

void ScrollBar::update()
{
    if (scrollBar->maximum() == 0)
    {
        position = -1;
    }
    else
    {
        ignored = true;
        if (position < 0)
        {
            scrollBar->setValue(static_cast<int>(scrollBar->maximum() / 2));
        }
        else
        {
            scrollBar->setValue(static_cast<int>(scrollBar->maximum() * position));
        }
        ignored = false;
    }
}

void ScrollBar::valueChanged()
{
    if (ignored == true)
    {
        return;
    }
    position = static_cast<qreal>(scrollBar->value()) / scrollBar->maximum();
}

GraphicsView::GraphicsView(QWidget *parent) :
    QGraphicsView(parent),
    hScrollBar(horizontalScrollBar()),
    vScrollBar(verticalScrollBar())
{
}

void GraphicsView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    updateScrollBars();
}

void GraphicsView::updateScrollBars()
{
    hScrollBar.update();
    vScrollBar.update();

}
