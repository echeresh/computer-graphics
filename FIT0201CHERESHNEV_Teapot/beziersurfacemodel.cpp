#include <QFile>
#include <QTextStream>
#include <cfloat>
#include <qmath.h>
#include "beziersurfacemodel.h"

const static int SURFACE_ORDER = 3; //bicubic Bézier surfaces

BezierPatch::BezierPatch() :
	N(SURFACE_ORDER),
	M(SURFACE_ORDER),
	controlPoints(M + 1, N + 1)
{
}

BezierPatch& BezierPatch::operator=(const BezierPatch& patch)
{
	controlPoints = patch.controlPoints;
	M = patch.M;
	N = patch.N;
	return *this;
}

BezierSurfaceModel::BezierSurfaceModel()
{
}

BezierSurfaceModel::BezierSurfaceModel(const QString& fileName)
{
	QFile file(fileName);
	QTextStream stream(&file);
	if (file.open(QIODevice::ReadOnly))
	{
		int nPatches;
		stream>>nPatches;
		for (int i = 0; i < nPatches; i++)
		{
			int n, m;
			stream>>n>>m;
			if (n != SURFACE_ORDER || m != SURFACE_ORDER)
			{
				patches.clear();
				return;
			}
			patches.append(BezierPatch());
			for (int j = 0; j < m + 1; j++)
			{
				for (int k = 0; k < n + 1; k++)
				{
					float x, y, z;
					stream>>x>>y>>z;
					patches[i].controlPoints(j, k) = QVector3D(x, y, z);
				}
			}
		}
	}
}

QVector<BezierGrid> BezierSurfaceModel::getGrids(int nSegments) const
{
	nSegments += 2;
	const float STEP = 1.f / (nSegments - 1);
	QVector<BezierGrid> grids(patches.count());
	for (int iPatch = 0; iPatch < patches.count(); iPatch++)
	{
		float u = 0.f;
		Matrix<QVector3D> patchGrid(nSegments, nSegments);
		for (int i = 0; i < nSegments; i++, u += STEP)
		{
			float v = 0.f;
			for (int j = 0; j < nSegments; j++, v += STEP)
			{
				patchGrid(i, j) = patches[iPatch].calcPoint(u, v);
			}
		}
		grids[iPatch] = BezierGrid(patchGrid);
	}
	return grids;
}

void BezierSurfaceModel::draw(int nSegments, Painter3D& painter) const
{
	nSegments += 2;
	const float STEP = 1.f / (nSegments - 1);
	for (int iPatch = 0; iPatch < patches.count(); iPatch++)
	{

		float u = 0.f;
		for (int i = 0; i < nSegments; i++, u += STEP)
		{
			QVector3D uControlPoints[SURFACE_ORDER + 1];
			for (int l = 0; l <= patches[iPatch].N; l++)
			{
				for (int k = 0; k <= patches[iPatch].M; k++)
				{
					uControlPoints[l] += Utils::bernsteinPolynomial(u, patches[iPatch].M, k) *
							 patches[iPatch].controlPoints(k, l);
				}
			}
			painter.drawCubicBezier(uControlPoints);
		}
		float v = 0.f;
		for (int j = 0; j < nSegments; j++, v += STEP)
		{
			QVector3D vControlPoints[SURFACE_ORDER + 1];
			for (int k = 0; k <= patches[iPatch].M; k++)
			{
				for (int l = 0; l <= patches[iPatch].N; l++)
				{
					vControlPoints[k] += Utils::bernsteinPolynomial(v, patches[iPatch].N, l) *
										patches[iPatch].controlPoints(k, l);
				}
			}
			painter.drawCubicBezier(vControlPoints);
		}
	}
}

QVector3D BezierPatch::calcPoint(float u, float v) const
{
	QVector3D p;
	for (int k = 0; k <= M; k++)
	{
		for (int l = 0; l <= N; l++)
		{
			p += Utils::bernsteinPolynomial(u, M, k) * Utils::bernsteinPolynomial(v, N, l) *
					controlPoints(k, l);
		}
	}
	return p;
}

QVector3D BezierSurfaceModel::center()
{
	QVector3D vMax(FLT_MIN, FLT_MIN, FLT_MIN);
	QVector3D vMin(FLT_MAX, FLT_MAX, FLT_MAX);
	const int MAX_SEGMENTS = 32;
	int nSegments = MAX_SEGMENTS;
	const float STEP = 1.f / (nSegments - 1);
	for (int iPatch = 0; iPatch < patches.count(); iPatch++)
	{
		float u = 0.f;
		for (int i = 0; i < nSegments; i++, u += STEP)
		{
			float v = 0.f;
			for (int j = 0; j < nSegments; j++, v += STEP)
			{
				QVector3D p = patches[iPatch].calcPoint(u, v);
				vMin = Utils::min(p, vMin);
				vMax = Utils::max(p, vMax);
			}
		}
	}
	QVector3D center((vMax + vMin) / 2);
	for (int iPatch = 0; iPatch < patches.count(); iPatch++)
	{
		for (int i = 0; i <= patches[iPatch].M; i++)
		{
			for (int j = 0; j <= patches[iPatch].N; j++)
			{
				patches[iPatch].controlPoints(i, j) -= center;
			}
		}
	}
	return vMax - center;
}

bool BezierSurfaceModel::isEmpty() const
{
	return patches.isEmpty();
}
