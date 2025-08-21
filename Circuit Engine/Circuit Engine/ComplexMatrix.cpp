#include "MathVector.h"
#include "ComplexMatrix.h"
#include "Complex.h"
#include <iostream>
#include "Constants.h"

using namespace std;

ComplexMatrix::~ComplexMatrix()
{
	if (data == nullptr) { return; }
	for (int r = 0; r < rows; r++)
	{
		if (data[r] == nullptr) { continue; }
		delete[] data[r];
	}

	delete[] data;
}

ComplexMatrix::ComplexMatrix(int _rows, int _columns)
{
	rows = _rows;
	cols = _columns;
	data = new complex* [rows];
	for (int r = 0; r < rows; r++)
	{
		data[r] = new complex[cols];
		for (int c = 0; c < cols; c++)
		{
			data[r][c] = 0;
		}
	}
}

ComplexMatrix::ComplexMatrix(ComplexMatrix* toCopy)
{
	rows = toCopy->rows;
	cols = toCopy->cols;
	data = new complex* [rows];
	for (int r = 0; r < rows; r++)
	{
		data[r] = new complex[cols];
		for (int c = 0; c < cols; c++)
		{
			data[r][c] = 0;
		}
	}

	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			insert(r, c, toCopy->get(r, c));
		}
	}
}

void ComplexMatrix::print()
{
	cout << "---ComplexMatrix---" << endl;
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			if (abs(data[r][c]) > ERROR_MARGIN)
			{
				cout << data[r][c] << " ";
			}
			else
			{
				cout << 0 << " ";
			}
			
		}
		cout << endl;
	}
	cout << "-----------" << endl;
}

void ComplexMatrix::map(vector<vector<complex>> vec)
{
	for (int r = 0; r < vec.size(); r++)
	{
		for (int c = 0; c < vec[r].size(); c++)
		{
			insert(r, c, vec[r][c]);
		}
	}
}

complex ComplexMatrix::get(int row, int col)
{
	if (row >= rows || col >= cols) { return 0; }
	return data[row][col];
}

ComplexMatrix* ComplexMatrix::getRow(int row)
{
	ComplexMatrix* rowVector = new ComplexMatrix(1, cols);
	for (int c = 0; c < cols; c++)
	{
		rowVector->insert(0, c, get(row, c));
	}
	return rowVector;
}

ComplexMatrix* ComplexMatrix::getCol(int col)
{
	ComplexMatrix* colVector = new ComplexMatrix(rows, 1);
	for (int r = 0; r < rows; r++)
	{
		colVector->insert(r, 0, get(r, col));
	}
	return colVector;
}

void ComplexMatrix::insert(int row, int col, complex value)
{
	if (row >= rows || col >= cols) { return; }
	data[row][col] = value;
}

void ComplexMatrix::addTo(int row, int col, complex value)
{
	insert(row, col, get(row, col) + value);
}

void ComplexMatrix::linearRowOperation(int sourceRow, int targetRow, complex scale)
{
	for (int c = 0; c < cols; c++)
	{
		data[targetRow][c] += data[sourceRow][c] * scale;
	}
}

void ComplexMatrix::linearRowScale(int row, complex scale)
{
	for (int c = 0; c < cols; c++)
	{
		data[row][c] *= scale;
	}
}

void ComplexMatrix::zeroRow(int row)
{
	for (int c = 0; c < cols; c++)
	{
		data[row][c] = 0;
	}
}

void ComplexMatrix::addRow()
{
	rows++;
	complex** newData = new complex* [rows];
	for (int r = 0; r < rows - 1; r++)
	{
		newData[r] = data[r];
	}
	newData[rows - 1] = new complex[cols];
	delete data;
	data = newData;
}

ComplexMatrix* ComplexMatrix::RREF()
{
	ComplexMatrix* m = new ComplexMatrix(this);
	for (int sourceRow = 0; sourceRow < rows; sourceRow++) //go through columns bounded by number of rows
	{
		int sourceCol = 0;
		while (sourceCol < rows && abs(m->get(sourceRow, sourceCol)) < ERROR_MARGIN) { sourceCol++; }
		if (abs(m->get(sourceRow, sourceCol)) < ERROR_MARGIN) { continue; }
		m->linearRowScale(sourceRow, 1.0 / m->get(sourceRow, sourceCol));
		for (int r = 0; r < rows; r++)
		{
			if (r == sourceRow) { continue; }

			complex scale = m->get(r, sourceCol);
			m->linearRowOperation(sourceRow, r, -scale);

		}
	}
	return m;
}

bool ComplexMatrix::rowExists(complex* rowVector)
{
	for (int r = 0; r < rows; r++)
	{
		bool matches = true;
		for (int c = 0; c < cols; c++)
		{
			if (data[r][c] != rowVector[c])
			{
				matches = false;
				break;
			}
		}
		if (matches) { return true; }
	}
	return false;
}

bool ComplexMatrix::linearCombinationExists(complex* rowVector)
{
	MVector reVector(rowVector, 0, cols);
	MVector imVector(rowVector, 1, cols);
	Matrix* realMatrix = new Matrix(cols, rows + 1);
	Matrix* imaginaryMatrix = new Matrix(cols, rows + 1);

	for (int c = 0; c < cols; c++)
	{
		for (int r = 0; r < rows; r++)
		{
			realMatrix->insert(c, r, get(r, c).real);
			imaginaryMatrix->insert(c, r, get(r, c).imaginary);
		}
		realMatrix->insert(c, rows, rowVector[c].real);
		imaginaryMatrix->insert(c, rows, rowVector[c].imaginary);
	}

	Matrix* reRREF = realMatrix->RREF();
	Matrix* imRREF = imaginaryMatrix->RREF();

	
	
	bool realLC = (realMatrix->isZeroMatrix() && reVector.magnitude() <= ERROR_MARGIN) || reRREF->isProperRREF();
	bool imaginaryLC = (imaginaryMatrix->isZeroMatrix() && imVector.magnitude() <= ERROR_MARGIN) || imRREF->isProperRREF();

	delete realMatrix;
	delete imaginaryMatrix;
	delete reRREF;
	delete imRREF;

	return realLC && imaginaryLC;
}

ComplexMatrix* ComplexMatrix::operator*(ComplexMatrix& rhs)
{
	ComplexMatrix* results = new ComplexMatrix(rows, rhs.cols);

	if (cols != rhs.rows) { return results; }

	for (int row = 0; row < results->rows; row++)
	{
		for (int col = 0; col < results->cols; col++)
		{
			results->insert(row, col, 0);

			for (int c = 0; c < cols; c++)
			{
				results->addTo(row, col, get(row, c) * rhs.get(c, col));
			}
		}
	}

	return results;

}

void ComplexMatrix::setRow(int row, complex* rowVector)
{
	for (int c = 0; c < cols; c++)
	{
		insert(row, c, rowVector[c]);
	}
}

bool ComplexMatrix::addLIRow(int row, complex* rowVector) //return true if successful
{
	if (!linearCombinationExists(rowVector) && !MVector::isComplexZeroVector(rowVector, cols))
	{
		setRow(row, rowVector);
		return true;
	}
	return false;
}

Matrix* ComplexMatrix::getReal()
{
	Matrix* m = new Matrix(rows, cols);
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < rows; c++)
		{
			m->insert(r, c, get(r, c).real);
		}
	}
	return m;
}

Matrix* ComplexMatrix::getImaginary()
{
	Matrix* m = new Matrix(rows, cols);
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < rows; c++)
		{
			m->insert(r, c, get(r, c).imaginary);
		}
	}
	return m;
}