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