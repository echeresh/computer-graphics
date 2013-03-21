#include <QScrollBar>
#include "graphicsview.h"

GraphicsView::GraphicsView(QWidget *parent) :
    QGraphicsView(parent)
{
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), SLOT(horValueChanged()));
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), SLOT(vertValueChanged()));
    userChanging = true;
    horFactor = -1;
    vertFactor = -1;
}
void GraphicsView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    procResize(horizontalScrollBar(), &horFactor);
    procResize(verticalScrollBar(), &vertFactor);

}
void GraphicsView::procResize(QScrollBar *scrollBar, qreal *factor)
{
    if (scrollBar->maximum() != 0)
    {
        userChanging = false;
        if (*factor > 0)
        {
            scrollBar->setValue(static_cast<int>(scrollBar->maximum() * *factor));
        }
        else
        {
            scrollBar->setValue(static_cast<int>(scrollBar->maximum() * .5));
        }
    }
    else
    {
        *factor = -1;
    }
}

void GraphicsView::horValueChanged()
{
    QScrollBar *scrollBar = horizontalScrollBar();
    if (userChanging == false)
    {
        userChanging = true;
    }
    else
    {
        horFactor = scrollBar->value() * 1. / scrollBar->maximum();
    }
}
void GraphicsView::vertValueChanged()
{
    QScrollBar *scrollBar = verticalScrollBar();
    if (userChanging == false)
    {
        userChanging = true;
    }
    else
    {
        vertFactor = scrollBar->value() * 1. / scrollBar->maximum();
    }
}
