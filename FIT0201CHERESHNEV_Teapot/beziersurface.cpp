#include <QFile>
#include <QTextStream>
#include <qmath.h>
#include "beziersurface.h"

static const int BIN_COEFS[5][5] =
{
	{1, 0, 0, 0, 0},
	{1, 1, 0, 0, 0},
	{1, 2, 1, 0, 0},
	{1, 3, 3, 1, 0},
	{1, 4, 6, 4, 1},
};

BezierPatch::BezierPatch(int n, int m) :
	N(n),
	M(m),
	controlPoints(m + 1, n + 1)
{
}

BezierPatch& BezierPatch::operator=(const BezierPatch& patch)
{
	controlPoints = patch.controlPoints;
	M = patch.M;
	N = patch.N;
	return *this;
}

BezierSurface::BezierSurface(QString fileName)
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
			patches.append(BezierPatch(n, m));
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

QVector<BezierGrid> BezierSurface::getGrids(int nSegments)
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
				QVector3D point;
				for (int k = 0; k <= patches[iPatch].M; k++)
				{
					for (int l = 0; l <= patches[iPatch].N; l++)
					{
						point += BIN_COEFS[patches[iPatch].M][k] * qPow(u, static_cast<float>(k)) *
																   qPow(1 - u, static_cast<float>(patches[iPatch].M - k)) *
								 BIN_COEFS[patches[iPatch].N][l] * qPow(v, static_cast<float>(l)) *
																   qPow(1 - v, static_cast<float>(patches[iPatch].N - l)) *
								 patches[iPatch].controlPoints(k, l);
					}
				}
				patchGrid(i, j) = point;
			}
		}
		grids[iPatch] = BezierGrid(patchGrid);
	}
	return grids;
}
