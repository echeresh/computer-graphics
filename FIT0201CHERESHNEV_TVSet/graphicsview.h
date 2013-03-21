#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>

class ScrollBar : QObject
{
    Q_OBJECT
public:

    ScrollBar(QScrollBar *scrollBar);
    void ignoreValueChange();
    void update();
private:
    QScrollBar *scrollBar;
    qreal position; //[0..1]
    bool ignored;

private slots:
    void valueChanged();
};

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsView(QWidget *parent = 0);
    void resizeEvent(QResizeEvent *event);
    void updateScrollBars();

private:
    ScrollBar hScrollBar;
    ScrollBar vScrollBar;
};

#endif // GRAPHICSVIEW_H
