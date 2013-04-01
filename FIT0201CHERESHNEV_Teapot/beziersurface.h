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
	BezierPatch(int n = 3, int m = 3);
	BezierPatch& operator=(const BezierPatch& patch);
};

class BezierSurface
{
public:
	BezierSurface(QString fileName);
	QVector<BezierGrid> getGrids(int nSegments);

private:
	QVector<BezierPatch> patches;
};

#endif // BEZIERSURFACE_H
