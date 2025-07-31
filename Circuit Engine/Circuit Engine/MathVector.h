#pragma once

#include <cstdarg>
#include <vector>

using namespace std;

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

class Matrix
{
private:
	double** data;
public:
	int rows, cols = 0;
	Matrix(int _rows, int _columns);

	void print();

	void map(vector<vector<double>> vec);

	void insert(int row, int col, double value);
	double get(int row, int col);

	void linearRowOperation(int sourceRow, int targetRow, double scale);
	void linearRowScale(int row, double scale);

	Matrix* RREF();
};