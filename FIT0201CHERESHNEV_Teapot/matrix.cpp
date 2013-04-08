#include "matrix.h"

template<class T>
Matrix<T>::Matrix(const Matrix& matrix) :
	nRows(matrix.nRows),
	nCols(matrix.nCols)
{
	elems = matrix.elems;
}
template<class T>
Matrix<T>::Matrix(unsigned nRows, unsigned nCols) :
	nRows(nRows),
	nCols(nCols)
{
	Q_ASSERT(nRows != 0 && nCols != 0);
	elems.resize(nRows);
	for (unsigned i = 0; i < nRows; i++)
	{
		elems[i].resize(nCols);
	}
}
template<class T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& matrix)
{
	elems = matrix.elems;
	nRows = matrix.nRows;
	nCols = matrix.nCols;
	return *this;
}

template<class T>
const T& Matrix<T>::operator()(unsigned row, unsigned col) const
{
	Q_ASSERT(row < nRows && col < nCols);
	return 	elems[row][col];
}

template<class T>
T& Matrix<T>::operator()(unsigned row, unsigned col)
{
	Q_ASSERT(row < nRows && col < nCols);
	return 	elems[row][col];
}

template<class T>
int Matrix<T>::rows() const
{
	return nRows;
}

template<class T>
int Matrix<T>::cols() const
{
	return nCols;
}
