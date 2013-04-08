#ifndef MATRIX_H
#define MATRIX_H

#include <QVector>

template<class T>
class Matrix
{
public:
	Matrix(const Matrix& matrix);
	Matrix(unsigned nRows = 1, unsigned nCols = 1);
	Matrix& operator=(const Matrix& matrix);
	const T& operator()(unsigned row, unsigned col) const;
	T& operator()(unsigned row, unsigned col);
	int rows() const;
	int cols() const;

private:
	QVector<QVector<T> > elems;
	unsigned nRows;
	unsigned nCols;
};
#include "matrix.cpp"

#endif // MATRIX_H

