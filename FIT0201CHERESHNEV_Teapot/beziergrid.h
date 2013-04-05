#ifndef BEZIERGRID_H
#define BEZIERGRID_H

#include <QVector3D>
#include <QVector>
#include "matrix.h"
#include "painter.h"

struct Triangle;

class BezierGrid
{
public:
	BezierGrid();
	BezierGrid(const Matrix<QVector3D> &points);
	BezierGrid(const BezierGrid& grid);
	BezierGrid& operator=(const BezierGrid& grid);
	QVector<Utils::Triangle3D> getTriangles(const Painter3D& painter) const;
	int rows() const;
	int cols() const;
	void center(const QVector3D& center);

private:
	Matrix<QVector3D> points;
};

#endif // BEZIERGRID_H
