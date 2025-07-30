#pragma once

#include <cstdarg>

class MVector
{
private:
	int size;
	double* values;
public:
	MVector(int count, ...);

	MVector(double* _values, int len);

	int get_size();
	void print();

	double operator[](int index);
	MVector operator+(MVector rhs);

};
