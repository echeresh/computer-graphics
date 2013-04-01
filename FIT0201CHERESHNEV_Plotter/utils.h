#ifndef UTILS_H
#define UTILS_H
#include <QPoint>
#include <QPointF>

namespace Utils
{
    int sign(int value);
    qreal norm(const QPointF& point);
    int normSquared(const QPoint& point);
    qreal normSquared(const QPointF& point);
    QPoint roundPoint(const QPointF& point);
}
#endif // UTILS_H
