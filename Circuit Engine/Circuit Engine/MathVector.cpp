#include "MathVector.h"
#include <iostream>

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
	print();
}

MVector::MVector(double* _values, int len)
{
	size = len;

	for (int i = 0; i < len; i++)
	{
		values.push_back(_values[i]);
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

void Matrix::print()
{
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			cout << data[r][c] << " ";
		}
		cout << endl;
	}
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

void Matrix::insert(int row, int col, double value)
{
	if (row >= rows || col >= cols) { return; }
	data[row][col] = value;
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

Matrix* Matrix::RREF()
{
	Matrix* m = new Matrix(*this);
	for (int c = 0; c < rows; c++) //go through columns bounded by number of rows
	{
		int sourceRow = c;
		linearRowScale(sourceRow, 1.0 / get(sourceRow, c));

		for (int r = 0; r < rows; r++)
		{
			if (r == sourceRow) { continue; }
			
			double scale = get(r, c);
			m->linearRowOperation(sourceRow, r, -scale);
		}
	}
	return m;
}