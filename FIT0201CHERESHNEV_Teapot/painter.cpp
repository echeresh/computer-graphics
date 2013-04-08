#include <QTime>
#include <QQueue>
#include <QMap>
#include <QMatrix4x4>
#include <QColor>
#include <qmath.h>
#include "painter.h"
#include "utils.h"

//projective plane - eye * R; between (0,0,0) and camera(= -eye)
static const float RMIN = .05f;
static const float RMAX = .95f;
static const float RSTEP = .025f;
static const float RINIT = .5f;

static const float INIT_SCALE = 1.f;
static const QVector3D INIT_EYE(0.f, -1.f, 0.f);
static const QVector3D INIT_UP(0.f, 0.f, 1.f);

Painter3D::Painter3D() :
	R(RINIT),
	scale(INIT_SCALE),
	eye(INIT_EYE),
	up(INIT_UP),
	bezierDrawing(ITERATIVE_BEZIER_DRAWING)
{
}

float Painter3D::distanceFromCamera(const QVector3D& v) const
{
	return (v + eye).length();
}

void Painter3D::drawLine(const QPoint& p0, const QPoint& p1)
{
	//Bresenham's line algorithm
	int dx = qAbs(p1.x() - p0.x());
	int dy = qAbs(p1.y() - p0.y());
	QPoint sp(Utils::sign(p1.x() - p0.x()), Utils::sign(p1.y() - p0.y()));
	QPoint ip(sp);
	if (dx > dy)
	{
		ip.setY(0);
		sp.setX(0);
	}
	else
	{
		qSwap(dx, dy);
		ip.setX(0);
		sp.setY(0);
	}
	QPoint p(p0);
	for(int err = -dx / 2; ;)
	{
		setPixel(p);
		if (p == p1)
		{
			return;
		}
		err += dy;
		p += ip;
		if (err > 0)
		{
			p += sp;
			err -= dx;
		}
	}
}

void Painter3D::drawLine(const QVector3D &v0, const QVector3D &v1)
{
	Utils::Line3D line;
	line.p0.coord = v0;
	line.p1.coord = v1;
	line.p0.proj = getProjection(v0, &line.p0.isVisible);
	line.p1.proj = getProjection(v1, &line.p1.isVisible);
	drawLine(line);
}

void Painter3D::drawLine(const Utils::Line3D &line)
{
	if (line.p0.isVisible && line.p1.isVisible)
	{
		drawLine(line.p0.proj, line.p1.proj);
	}
	else if (!line.p0.isVisible && line.p1.isVisible)
	{
		drawLine(getIntersection(line.p0.coord, line.p1.coord), line.p1.proj);
	}
	else if (line.p0.isVisible && !line.p1.isVisible)
	{
		drawLine(getIntersection(line.p0.coord, line.p1.coord), line.p0.proj);
	}
}

void Painter3D::setBezierDrawing(BezierDrawing bezierDrawing)
{
	this->bezierDrawing = bezierDrawing;
}

void Painter3D::drawCubicBezier(const QVector3D* controlPoints)
{
	switch (bezierDrawing)
	{
		case PIXEL_BY_PIXEL_BEZIER_DRAWING:
			drawCubicBezierPixelByPixel(controlPoints);
			break;
		case ITERATIVE_BEZIER_DRAWING:
			drawCubicBezierIterative(controlPoints);
			break;
		case DE_CASTELJAU_BEZIER_DRAWING:
			drawCubicBezierRecursive(controlPoints);
			break;
	}
}

void Painter3D::drawCubicBezierPixelByPixel(const QVector3D *controlPoints)
{
	const int N = 3;
	typedef QPair<float, Utils::Point3D> BezierPoint;
	QQueue<QPair<BezierPoint, BezierPoint> > queue;

	BezierPoint bp0 = qMakePair(0.f, projectPoint(Utils::bezierValue(controlPoints, N, 0.f)));
	BezierPoint bp1 = qMakePair(1.f, projectPoint(Utils::bezierValue(controlPoints, N, 1.f)));
	queue.enqueue(qMakePair(bp0, bp1));
	if (bp0.second.isVisible)
	{
		setPixel(bp0.second.proj);
	}
	while (!queue.isEmpty())
	{
		QPair<BezierPoint, BezierPoint> range = queue.dequeue();
		if (range.second.second.isVisible)
		{
			setPixel(range.second.second.proj);
		}
		if (qAbs(range.first.second.proj.x() - range.second.second.proj.x()) > 1 ||
				qAbs(range.first.second.proj.y() - range.second.second.proj.y()) > 1)
		{
			float mt = (range.first.first + range.second.first) / 2;
			BezierPoint middle = qMakePair(mt, projectPoint(Utils::bezierValue(controlPoints, N, mt)));
			queue.enqueue(qMakePair(range.first, middle));
			queue.enqueue(qMakePair(middle, range.second));
		}
	}
}

void Painter3D::drawCubicBezierRecursive(const QVector3D* controlPoints, int depth)
{
	//De Casteljau's algorithm
	const int N = 3;
	const float tau = .5f;
	if (--depth == 0)
	{
		for (int i = 1; i <= N; i++)
		{
			drawLine(controlPoints[i - 1], controlPoints[i]);
		}
		return;
	}
	QVector3D p[N + 1][N + 1];
	for (int i = 0; i <= N; i++)
	{
		p[i][0] = controlPoints[i];
	}
	QVector3D controlPoints0[N + 1];
	QVector3D controlPoints1[N + 1];
	controlPoints0[0] = p[0][0];
	controlPoints1[N] = p[N][0];
	for (int j = 1; j <= N; j++)
	{
		for (int i = 0; i <= N - j; i++)
		{
			p[i][j] = (1 - tau) * p[i][j - 1] + tau * p[i + 1][j - 1];
		}
		controlPoints0[j] = p[0][j];
		controlPoints1[N - j] = p[N - j][j];
	}
	drawCubicBezierRecursive(controlPoints0, depth);
	drawCubicBezierRecursive(controlPoints1, depth);
}

void Painter3D::drawCubicBezierIterative(const QVector3D* controlPoints, float step)
{
	const int N = 3;
	Utils::Point3D p0 = projectPoint(Utils::bezierValue(controlPoints, N, 0.f));
	for (float t = step, to = 1 + step / 2; t < to; t += step)
	{
		Utils::Point3D p1 = projectPoint(Utils::bezierValue(controlPoints, N, t));
		Utils::Line3D line = {p0, p1};
		drawLine(line);
		p0 = p1;
	}
}

QPoint Painter3D::getProjection(const QVector3D& v, bool *isCross) const
{
	return getIntersection(v, -eye, isCross);
}

QPoint Painter3D::getIntersection(const QVector3D& v0, const QVector3D& v1, bool *isCross) const
{
	QVector3D camera(-eye);
	QVector3D dv(v0 - v1);
	float d = QVector3D::dotProduct(R * camera - v1, eye) /	QVector3D::dotProduct(dv, eye);
	QVector3D p = v1 + d * dv;
	if (isCross != NULL)
	{
		*isCross = Utils::sign((v0 - p).x() / dv.x()) != Utils::sign(-d);
	}
	return to2D(p);
}

Utils::Point3D Painter3D::projectPoint(const QVector3D& v)
{
	Utils::Point3D p3d;
	p3d.coord = v;
	p3d.proj = getProjection(p3d.coord, &p3d.isVisible);
	return p3d;
}

void Painter3D::xRotate(float angle)
{
	QMatrix4x4 xMatrix;
	xMatrix.rotate(angle, up);
	eye = xMatrix.map(eye);
}

void Painter3D::yRotate(float angle)
{
	QMatrix4x4 yMatrix;
	yMatrix.rotate(angle, QVector3D::crossProduct(up, eye));
	eye = yMatrix.map(eye);
	up = yMatrix.map(up);
}

void Painter3D::recalcView(const QVector3D& boundingBox)
{
	const float VIEW_WIDTH = 400.f;
	scale = VIEW_WIDTH / boundingBox.x();
	eye.normalize();
	eye *= 2 * boundingBox.length();
}

void Painter3D::zoom(int dr)
{
	float newR = R - dr * RSTEP;
	newR = qMax(newR, RMIN);
	R = qMin(newR, RMAX);
}

QPoint Painter3D::to2D(const QVector3D& v) const
{
	QVector3D right = -QVector3D::crossProduct(eye, up);
	right.normalize();
	//x = (v + eye * R, right) but (eye, right) = 0 hence x = (v, right), y - the same
	int x = qRound(scale * QVector3D::dotProduct(v, right));
	int y = qRound(scale * QVector3D::dotProduct(v, up));
	return QPoint(buffer.width() / 2 + x, buffer.height() / 2 - y);
}

void Painter3D::drawBox(const QVector3D& diag, const QVector3D& center)
{
	const int VERTEX_COUNT = 8;
	for (int i = 0; i < VERTEX_COUNT; i++)
	{
		for (int j = i + 1; j < VERTEX_COUNT; j++)
		{
			int x0 = (i & 1) == 0 ? -1 : 1, x1 = (j & 1) == 0 ? -1 : 1;
			int y0 = (i & 2) == 0 ? -1 : 1, y1 = (j & 2) == 0 ? -1 : 1;
			int z0 = (i & 4) == 0 ? -1 : 1, z1 = (j & 4) == 0 ? -1 : 1;
			if (qAbs(x1 - x0) + qAbs(y1 - y0) + qAbs(z1 - z0) == 2)
			{
				drawLine(center + QVector3D(diag.x() * x0, diag.y() * y0, diag.z() * z0),
						 center + QVector3D(diag.x() * x1, diag.y() * y1, diag.z() * z1));
			}
		}
	}
}

void Painter3D::drawTriangles(const QVector<Utils::Triangle3D> &triangles, bool showFrame, QRgb frameColor,
							  bool showFilling, QRgb fillColor, bool showLight)
{
	QVector<QPair<float, int> > distances(triangles.count());
	for (int i = 0; i < triangles.count(); i++)
	{
		const Utils::Triangle3D& t = triangles[i];
		distances[i].first =  distanceFromCamera((t.p0.coord + t.p1.coord + t.p2.coord) / 3);
		distances[i].second = i;
	}

	//Painter's algorithm
	struct Compare
	{
		static bool compare(const QPair<float, int>& pair0, const QPair<float, int>& pair1)
		{
			return pair0.first > pair1.first;
		}
	};
	qSort(distances.begin(), distances.end(), Compare::compare);

	qreal dMin = static_cast<float>(distances.last().first);
	qreal dMax = static_cast<float>(distances.first().first);
	setColor(frameColor);
	for (int i = 0; i < distances.count(); i++)
	{
		const Utils::Triangle3D& triangle = triangles[distances[i].second];
		if (showFilling)
		{
			if (showLight)
			{
				//empirical constants
				const qreal C0 = 1.5;
				const qreal C1 = 1.2;
				const qreal C2 = 100.;
				qreal factor = 1 + C0 * qPow(((distances[i].first - dMin) / (dMax - dMin)), C1);
				setColor(QColor(fillColor).darker(static_cast<int>(C2 * factor)).rgba());
			}
			else
			{
				setColor(fillColor);
			}
			fillTriangle(triangle);
			setColor(frameColor);
		}
		if (showFrame)
		{
			Utils::Line3D line0 = {triangle.p0, triangle.p1};
			Utils::Line3D line1 = {triangle.p1, triangle.p2};
			drawLine(line0);
			drawLine(line1);
		}
	}
}

void Painter3D::fillTriangle(const Utils::Triangle3D &triangle)
{
	const QVector3D v[3] = {triangle.p0.coord, triangle.p1.coord, triangle.p2.coord};
	const int MAX_POINTS = 4; //max number of points when is crossed by plane
	QPoint p[3] = {triangle.p0.proj, triangle.p1.proj, triangle.p2.proj};
	bool visible[3] = {triangle.p0.isVisible, triangle.p1.isVisible, triangle.p2.isVisible};
	QPoint points[MAX_POINTS];
	int iPoint = 0;
	for (int i = 0, iPrev = 2; i < 3; i++, iPrev = (iPrev + 1) % 3)
	{
		if (visible[iPrev] ^ visible[i])
		{
			points[iPoint++] = getIntersection(v[iPrev], v[i]);
		}
		if (visible[i])
		{
			points[iPoint++] = p[i];
		}
	}
	if (iPoint == MAX_POINTS)
	{
		//triangle -> quadrilateral -> 2 triangles (when crossed by projective plane)
		fillTriangle(points[0], points[1], points[2]);
		fillTriangle(points[2], points[3], points[0]);
	}
	else if (iPoint == 3)
	{
		fillTriangle(points[0], points[1], points[2]);
	}
}

void Painter3D::fillTriangle(QPoint p0, QPoint p1, QPoint p2)
{
	if (p1.y() <= p0.y() && p1.y() <= p2.y())
	{
		qSwap(p0, p1);
	}
	else if (p2.y() <= p0.y() && p2.y() <= p1.y())
	{
		qSwap(p0, p2);
	}
	LineDrawer drawer0(p0, p1);
	LineDrawer drawer1(p0, p2);
	QPoint np0(0, p0.y() + 1);
	QPoint np1(0, p0.y() + 1);
	for(int y = p0.y(), yMax = qMax(p1.y(), p2.y()); y <= yMax; y++)
	{
		while (np0.y() != y)
		{
			if (drawer0.next(np0) == false)
			{
				drawer0 = LineDrawer(p1, p2);
			}
		}
		while (np1.y() != y)
		{
			if (drawer1.next(np1) == false)
			{
				drawer1 = LineDrawer(p2, p1);
			}
		}
		for (int x = qMin(np0.x(), np1.x()), xMax = qMax(np0.x(), np1.x()); x <= xMax; x++)
		{
			setPixel(QPoint(x, y));
		}
	}
}

void Painter3D::resizeBuffer(const QSize &newSize)
{
	buffer = QImage(newSize, QImage::Format_RGB888);
}

void Painter3D::setPixel(const QPoint& p)
{
	if (buffer.rect().contains(p))
	{
		buffer.setPixel(p, rgb);
	}
}

void Painter3D::setColor(QRgb newRgb)
{
	rgb = newRgb;
}

LineDrawer::LineDrawer(const QPoint& p0, const QPoint& p1)
{
	this->p1 = p1;
	dx = qAbs(p1.x() - p0.x());
	dy = qAbs(p1.y() - p0.y());
	sp = QPoint(Utils::sign(p1.x() - p0.x()), Utils::sign(p1.y() - p0.y()));
	ip = sp;
	if (dx > dy)
	{
		ip.setY(0);
		sp.setX(0);
	}
	else
	{
		qSwap(dx, dy);
		ip.setX(0);
		sp.setY(0);
	}
	p = p0;
	err = -dx / 2;
}

bool LineDrawer::next(QPoint &nextPoint)
{
	nextPoint = p;
	if (p == p1)
	{
		return false;
	}
	err += dy;
	p += ip;
	if (err > 0)
	{
		p += sp;
		err -= dx;
	}
	return true;
}

