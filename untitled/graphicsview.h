#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsView(QWidget *parent = 0);
    void resizeEvent(QResizeEvent *event);
    void scrollToCenter();

signals:
private:
    void procResize(QScrollBar *scrollBar, qreal *factor);
private slots:
    void horValueChanged();
    void vertValueChanged();

private:
    bool userChanging;
    qreal horFactor;
    qreal vertFactor;
};

#endif // GRAPHICSVIEW_H
