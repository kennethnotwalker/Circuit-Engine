#pragma once

#include <cstdarg>
#include <vector>

class MVector
{
private:
	int size;
	std::vector<double> values = {};
public:
	MVector(int count, ...);

	MVector(double* _values, int len);

	~MVector();

	int get_size();
	void print();

	double operator[](int index);
	MVector operator+(MVector rhs);
	MVector operator-(MVector rhs);
	MVector operator*(double rhs);
	double operator*(MVector rhs);

};
