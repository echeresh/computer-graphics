#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include "quadranglemapper.h"
#include "mipmap.h"
#include "utils.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	
private:
	Ui::MainWindow *ui;
	QImage buffer;
	MipMap mipMap;

	int sideLength;
	QPoint cPoint;
	QPoint fPoint;

	QuadrangleMapper topQuadMapper;
	QuadrangleMapper bottomQuadMapper;

	void resizeBuffer();
	void updateBuffer();
	int mostLeftX();
	int mostRightX();
	QRect boundingRect();
	QPair<int, int> getOffsets();
	void createQuadrangles();
	Utils::Quadrangle topQuadrangle();
	Utils::Quadrangle bottomQuadrangle();
	void mousePressEvent(QMouseEvent *);

signals:
	void pointCChanged(int x);
	void pointFChanged(int x);

private slots:
	void movePointC(int pos);
	void movePointF(int pos);
	void setPointLayerFiltering();
	void setLinearLayerFiltering();
	void setNoneMipFiltering();
	void setPointMipFiltering();
	void setLinearMipFiltering();
	void setWidth(int newWidth);
	void openTexture();
	void setDefaultViewMode();
	void setUVViewMode();
	void setMIPViewMode();
	void setMaxMipLevel(int maxLevel);
};

#endif // MAINWINDOW_H
