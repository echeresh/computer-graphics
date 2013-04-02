#include <QDebug>
#include <limits.h>
#include "beziergrid.h"
#include "utils.h"

BezierGrid::BezierGrid()
{
}

BezierGrid::BezierGrid(const Matrix<QVector3D> &points) :
	points(points)
{
}

BezierGrid::BezierGrid(const BezierGrid& grid)
{
	points = grid.points;
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

QVector3D BezierGrid::getMinPoint()
{
	QVector3D vMin(FLT_MAX, FLT_MAX, FLT_MAX);
	for (int i = 0; i < rows(); i++)
	{
		for (int j = 0; j < cols(); j++)
		{
			vMin = Utils::min(points(i, j), vMin);
		}
	}
	return vMin;
}

QVector3D BezierGrid::getMaxPoint()
{
	QVector3D vMax(FLT_MIN, FLT_MIN, FLT_MIN);
	for (int i = 0; i < rows(); i++)
	{
		for (int j = 0; j < cols(); j++)
		{
			vMax = Utils::max(points(i, j), vMax);
		}
	}
	return vMax;
}

void BezierGrid::center(const QVector3D& center)
{
	for (int i = 0; i < rows(); i++)
	{
		for (int j = 0; j < cols(); j++)
		{
			points(i, j) -= center;
		}
	}
}

void BezierGrid::draw(Projector& projector)
{
	int nRows = rows();
	int nCols = cols();
	for (int i = 0; i < nRows - 1; i++)
	{
		for (int j = 0; j < nCols - 1; j++)
		{
			projector.drawLine(points(i, j), points(i + 1, j));
			projector.drawLine(points(i, j), points(i, j + 1));

			projector.drawLine(points(nRows - 1, j), points(nRows - 1, j + 1));
		}
		projector.drawLine(points(i, nCols - 1), points(i + 1, nCols - 1));
	}
}
