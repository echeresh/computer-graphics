#include "beziergrid.h"
#include "utils.h"

BezierGrid::BezierGrid()
{
}

BezierGrid::BezierGrid(const Matrix<QVector3D> &points) :
	points(points)
{
}

BezierGrid::BezierGrid(const BezierGrid& grid) :
	points(grid.points)
{
}

BezierGrid& BezierGrid::operator=(const BezierGrid& grid)
{
	points = grid.points;
	return *this;
}

int BezierGrid::rows() const
{
	return points.rows();
}

int BezierGrid::cols() const
{
	return points.cols();
}

QVector<Utils::Triangle3D> BezierGrid::getTriangles(const Painter3D& painter) const
{
	const int nRows = rows();
	const int nCols = cols();

	QVector<Utils::Triangle3D> triangles;
	Matrix<Utils::Point3D> points3d(nRows, nCols);
	for (int i = 0; i < nRows; i++)
	{
		for (int j = 0; j < nCols; j++)
		{
			Utils::Point3D p3d;
			p3d.coord = points(i, j);
			p3d.proj = painter.getProjection(p3d.coord, &p3d.isVisible);
			points3d(i, j) = p3d;
		}
	}
	for (int i = 0; i < nRows - 1; i++)
	{
		for (int j = 0; j < nCols - 1; j++)
		{
			Utils::Triangle3D t0 = {points3d(i + 1, j), points3d(i, j), points3d(i, j + 1)};
			Utils::Triangle3D t1 = {points3d(i + 1, j), points3d(i + 1, j + 1), points3d(i, j + 1)};
			triangles.append(t0);
			triangles.append(t1);
		}
	}
	return triangles;
}
