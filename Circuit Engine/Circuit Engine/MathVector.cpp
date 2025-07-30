#include "MathVector.h"
#include <iostream>

using namespace std;

MVector::MVector(int count, ...)
{
	va_list args;
	size = count;
	va_start(args, count);
	values = new double[size];

	for (int i = 0; i < size; i++)
	{
		double v = va_arg(args, double);
		cout << v << endl;
		values[i] = v;
	}
}

MVector::MVector(double* _values, int len)
{
	size = len;

	values = new double[size];

	for (int i = 0; i < len; i++)
	{
		values[i] = _values[i];
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
		cout << values[i] << endl;
	}
}

MVector MVector::operator+(MVector rhs)
{
	MVector lhs = *this;

	double* results = new double[lhs.get_size()];

	for (int i = 0; i < lhs.get_size(); i++)
	{
		results[i] = lhs[i] + rhs[i];
	}

	return MVector(results, lhs.get_size());
}

double MVector::operator[](int index)
{
	return (this->values)[index];
}