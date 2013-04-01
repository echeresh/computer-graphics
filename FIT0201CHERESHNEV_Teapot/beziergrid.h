#ifndef BEZIERGRID_H
#define BEZIERGRID_H

#include <QVector3D>
#include <QVector>
#include "matrix.h"
#include "projector.h"

class BezierGrid
{
public:
	BezierGrid();
	BezierGrid(const Matrix<QVector3D> &points);
	BezierGrid(const BezierGrid& grid);
	BezierGrid& operator=(const BezierGrid& grid);
	int rows() const;
	int cols() const;

	QVector3D getMinPoint();
	QVector3D getMaxPoint();
	void center(const QVector3D& center);
	void draw(Projector& projector);

private:
	Matrix<QVector3D> points;
};

#endif // BEZIERGRID_H
