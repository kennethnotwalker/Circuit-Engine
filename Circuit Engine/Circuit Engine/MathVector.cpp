#include "MathVector.h"
#include <iostream>
#include "Complex.h"
#include "Constants.h"

using namespace std;

MVector::~MVector()
{
	values.clear();
}

MVector::MVector(int count, ...)
{
	va_list args;
	size = count;
	va_start(args, count);

	for (int i = 0; i < size; i++)
	{
		double v = va_arg(args, double);
		values.push_back(v);
	}
}

MVector::MVector(double* _values, int len)
{
	size = len;

	for (int i = 0; i < len; i++)
	{
		values.push_back(_values[i]);
	}
}

MVector::MVector(complex* _values, int axis, int len)
{
	size = len;

	for (int i = 0; i < len; i++)
	{
		double value = _values[i].real;
		if (axis == 1) { value = _values[i].imaginary; }
		values.push_back(value);
	}
}

int MVector::get_size()
{
	return size;
}

void MVector::print()
{
	for (int i = 0; i < size; i++)
	{
		cout << values[i];
		if (i < size - 1) { cout << ", "; }
	}
	cout << endl;
}

MVector MVector::operator+(MVector rhs)
{
	MVector lhs = *this;

	double* results = new double[lhs.get_size()];

	for (int i = 0; i < lhs.get_size(); i++)
	{
		results[i] = lhs[i] + rhs[i];
	}
	MVector res = MVector(results, lhs.get_size());
	delete [] results;
	return res;
}

MVector MVector::operator*(double rhs)
{
	MVector lhs = *this;

	double* results = new double[lhs.get_size()];

	for (int i = 0; i < lhs.get_size(); i++)
	{
		results[i] = lhs[i]*rhs;
	}
	MVector res = MVector(results, lhs.get_size());
	delete[] results;
	return res;
}

MVector MVector::operator-(MVector rhs)
{
	return (*this) + rhs*(-1);
}

double MVector::operator*(MVector rhs)
{
	MVector lhs = *this;

	double result = 0;

	for (int i = 0; i < lhs.get_size(); i++)
	{
		result += lhs[i] * rhs[i];
	}

	return result;
}

double MVector::operator[](int index)
{
	return (this->values)[index];
}

double MVector::magnitude()
{
	double mag = 0;
	for (int i = 0; i < size; i++)
	{
		mag += values[i] * values[i];
	}

	return sqrt(mag);
}

double MVector::distance(MVector A, MVector B)
{
	MVector displacement = B - A;
	return displacement.magnitude();
}

double MVector::cosBetween(MVector A, MVector B)
{
	if (isZeroVector(A.values.data(), A.size) && isZeroVector(B.values.data(), B.size)) { return 1; }
	double cosangle = (A * B) / (A.magnitude() * B.magnitude());

	return cosangle;
}

double MVector::angleBetween(MVector A, MVector B)
{
	double cosangle = cosBetween(A,B);
	double angle = acos(cosangle);

	return angle;
}

bool MVector::isZeroVector(double* nums, int _size)
{
	double tolerance = ERROR_MARGIN;
	for (int i = 0; i < _size; i++)
	{
		if (abs(nums[i]) > tolerance)
		{
			return false;
		}
	}
	return true;
}

bool MVector::isComplexZeroVector(complex* nums, int _size)
{
	double tolerance = ERROR_MARGIN;
	for (int i = 0; i < _size; i++)
	{
		if (abs(nums[i]) > tolerance)
		{
			return false;
		}
	}
	return true;
}

Matrix::~Matrix()
{
	if (data == nullptr) {return;}
	for (int r = 0; r < rows; r++)
	{
		if (data[r] == nullptr) { continue; }
		delete[] data[r];
	}

	delete[] data;
}

Matrix::Matrix(int _rows, int _columns)
{
	rows = _rows;
	cols = _columns;
	data = new double* [rows];
	for (int r = 0; r < rows; r++)
	{
		data[r] = new double[cols];
		for (int c = 0; c < cols; c++)
		{
			data[r][c] = 0;
		}
	}
}

Matrix::Matrix(Matrix* toCopy)
{
	rows = toCopy->rows;
	cols = toCopy->cols;
	data = new double* [rows];
	for (int r = 0; r < rows; r++)
	{
		data[r] = new double[cols];
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

void Matrix::print()
{
	cout << "---Matrix---" << endl;
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			cout << data[r][c] << " ";
		}
		cout << endl;
	}
	cout << "-----------" << endl;
}

void Matrix::map(vector<vector<double>> vec)
{
	for (int r = 0; r < vec.size(); r++)
	{
		for (int c = 0; c < vec[r].size(); c++)
		{
			insert(r, c, vec[r][c]);
		}
	}
}

double Matrix::get(int row, int col)
{
	if (row >= rows || col >= cols) { return 0; }
	return data[row][col];
}

Matrix* Matrix::getRow(int row)
{
	Matrix* rowVector = new Matrix(1, cols);
	for (int c = 0; c < cols; c++)
	{
		rowVector->insert(0, c, get(row, c));
	}
	return rowVector;
}

Matrix* Matrix::getCol(int col)
{
	Matrix* colVector = new Matrix(rows, 1);
	for (int r = 0; r < rows; r++)
	{
		colVector->insert(r, 0, get(r, col));
	}
	return colVector;
}

void Matrix::insert(int row, int col, double value)
{
	if (row >= rows || col >= cols) { return; }
	data[row][col] = value;
}

void Matrix::addTo(int row, int col, double value)
{
	insert(row, col, get(row, col) + value);
}

void Matrix::linearRowOperation(int sourceRow, int targetRow, double scale)
{
	for (int c = 0; c < cols; c++)
	{
		data[targetRow][c] += data[sourceRow][c] * scale;
	}
}

void Matrix::linearRowScale(int row, double scale)
{
	for (int c = 0; c < cols; c++)
	{
		data[row][c] *= scale;
	}
}

void Matrix::zeroRow(int row)
{
	for (int c = 0; c < cols; c++)
	{
		data[row][c] = 0;
	}
}

void Matrix::addRow()
{
	rows++;
	double** newData = new double* [rows];
	for (int r = 0; r < rows-1; r++)
	{
		newData[r] = data[r];
	}
	newData[rows - 1] = new double[cols];
	delete data;
	data = newData;
}

Matrix* Matrix::RREF()
{
	Matrix* m = new Matrix(this);
	for (int sourceRow = 0; sourceRow < rows; sourceRow++) //go through columns bounded by number of rows
	{
		int sourceCol = 0;
		while (sourceCol < rows && abs(m->get(sourceRow, sourceCol)) < ERROR_MARGIN) { sourceCol++; }
		if (abs(m->get(sourceRow, sourceCol)) < ERROR_MARGIN) { continue; }
		m->linearRowScale(sourceRow, 1.0 / m->get(sourceRow, sourceCol));
		for (int r = 0; r < rows; r++)
		{
			if (r == sourceRow) { continue; }
			
			double scale = m->get(r, sourceCol);
			m->linearRowOperation(sourceRow, r, -scale);
			
		}
	}
	return m;
}

bool Matrix::rowExists(double* rowVector)
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

bool Matrix::linearCombinationExists(double* rowVector)
{
	MVector rowMVector = MVector(rowVector, cols);

	for (int r = 0; r < rows; r++)
	{
		MVector currentVector = MVector(data[r], cols);

		double cosangle = MVector::cosBetween(rowMVector, currentVector);

		if (abs(abs(cosangle) - 1) < ERROR_MARGIN) { return true; }
	}
	return false;
}

Matrix* Matrix::operator*(Matrix& rhs)
{
	Matrix* results = new Matrix(rows, rhs.cols);

	if (cols != rhs.rows) { return results; }

	for (int row = 0; row < results->rows; row++)
	{
		for (int col = 0; col < results->cols; col++)
		{
			results->insert(row, col, 0);

			for (int c = 0; c < cols; c++)
			{
				results->addTo(row, col, get(row, c)*rhs.get(c, col));
			}
		}
	}

	return results;
	
}

void Matrix::setRow(int row, double* rowVector)
{
	for (int c = 0; c < cols; c++)
	{
		insert(row, c, rowVector[c]);
	}
}

bool Matrix::addLIRow(int row, double* rowVector) //return true if successful
{
	if (!linearCombinationExists(rowVector) && !MVector::isZeroVector(rowVector, cols))
	{
		setRow(row, rowVector);
		return true;
	}
	return false;
}

int Matrix::countInRow(int _row, double value, int _end, double tolerance)
{
	int count = 0;
	int end = _end;

	if (end == -1)
	{
		end = cols;
	}

	for (int c = 0; c < end; c++)
	{
		if (abs(get(_row, c) - value) <= tolerance)
		{
			count++;
		}
	}
	return count;
}

bool Matrix::isProperRREF()
{
	for (int r = 0; r < rows; r++)
	{
		if (countInRow(r, 1, cols - 1, 0) != 1 && countInRow(r, 0, cols, 0) != cols) { return false; }
	}
	return true;
}

bool Matrix::isZeroMatrix(double tolerance)
{
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			if (abs(get(r, c)) > tolerance)
			{
				return false;
			}
		}
	}
	return true;
}