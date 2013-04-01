#ifndef CASSINIWIDGET_H
#define CASSINIWIDGET_H

#include <QWidget>
#include <QImage>
#include "drawer.h"

class CassiniWidget : public QWidget, Drawer
{
    Q_OBJECT
public:
    explicit CassiniWidget(QWidget *parent = 0);
	void paintEvent(QPaintEvent *);
	void resizeEvent(QResizeEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
    
signals:
	void RChanged(const QString &RText);
	void focus0Changed(const QString &focus0Text);
	void focus1Changed(const QString &focus1Text);
	void thicknessChanged(const QString &thicknessText);

private:
	QImage templateBuffer;
	bool isMousePressed;
	QPoint focus0;
	QPoint focus1;
	int R;
	int thickness;

private:
	void drawAxis();
    void drawFocuses(const QPoint& f0, const QPoint& f1);
	void drawFocusLine(const QPoint& f0, const QPoint& f1);

	QPoint toRelative(const QPoint& point);
	QPoint toAbsolute(const QPoint& point);
	void updatePlot();

private slots:
	void updateR(int RValue);
	void updateThickness(int thicknessValue);
};

#endif // CASSINIWIDGET_H
