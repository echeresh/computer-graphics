#ifndef MATRIX_H
#define MATRIX_H

#include <QVector>

template<class T>
class Matrix
{
public:
	Matrix(const Matrix& matrix) :
		nRows(matrix.nRows),
		nCols(matrix.nCols)
	{
		elems = matrix.elems;
	}
	Matrix(int nRows = 1, int nCols = 1) :
		nRows(nRows),
		nCols(nCols)
	{
		Q_ASSERT(nRows > 0 && nCols > 0);
		elems.resize(nRows);
		for (int i = 0; i < nRows; i++)
		{
			elems[i].resize(nCols);
		}
	}
	Matrix& operator=(const Matrix& matrix)
	{
		elems = matrix.elems;
		nRows = matrix.nRows;
		nCols = matrix.nCols;
		return *this;
	}
	const T& operator()(int row, int col) const
	{
		Q_ASSERT(row >= 0 && row < nRows && col >= 0 && col < nCols);
		return 	elems[row][col];
	}
	T& operator()(int row, int col)
	{
		Q_ASSERT(row >= 0 && row < nRows && col >= 0 && col < nCols);
		return 	elems[row][col];
	}
	int rows() const
	{
		return nRows;
	}
	int cols() const
	{
		return nCols;
	}

private:
	QVector<QVector<T> > elems;
	int nRows;
	int nCols;
};

#endif // MATRIX_H

