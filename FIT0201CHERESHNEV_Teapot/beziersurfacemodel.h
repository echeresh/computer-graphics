#ifndef BEZIERSURFACE_H
#define BEZIERSURFACE_H

#include <QVector3D>
#include <QVector>
#include "beziergrid.h"
#include "matrix.h"

struct BezierPatch
{
	int N;
	int M;
	Matrix<QVector3D> controlPoints;
	BezierPatch();
	BezierPatch& operator=(const BezierPatch& patch);
	QVector3D calcPoint(float u, float v) const;
};

class BezierSurfaceModel
{
public:
	BezierSurfaceModel();
	BezierSurfaceModel(const QString& fileName);
	QVector<BezierGrid> getGrids(int nSegments) const;
	void draw(int nSegments, Painter3D& painter) const;
	bool isEmpty() const;
	QVector3D center();

private:
	QVector<BezierPatch> patches;
};

#endif // BEZIERSURFACE_H
