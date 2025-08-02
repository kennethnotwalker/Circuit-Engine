#pragma once

#include <cstdarg>
#include <vector>
#include "Complex.h"

using namespace std;

class MVector
{
private:
	int size;
	std::vector<double> values = {};
public:
	MVector(int count, ...);

	MVector(double* _values, int len);

	MVector(complex* _values, int axis, int len);

	~MVector();

	int get_size();
	void print();
	double magnitude();
	static double distance(MVector A, MVector B);
	static double angleBetween(MVector A, MVector B);
	static double cosBetween(MVector A, MVector B);

	double operator[](int index);
	MVector operator+(MVector rhs);
	MVector operator-(MVector rhs);
	MVector operator*(double rhs);
	double operator*(MVector rhs);

	static bool isZeroVector(double* nums, int _size);
	static bool isComplexZeroVector(complex* nums, int _size);

};

class Matrix
{
private:
	double** data;
public:
	int rows, cols = 0;
	Matrix(int _rows, int _columns);
	Matrix(Matrix* toCopy);
	~Matrix();

	void print();

	void map(vector<vector<double>> vec);

	void insert(int row, int col, double value);
	void addTo(int row, int col, double value);
	double get(int row, int col);

	void linearRowOperation(int sourceRow, int targetRow, double scale);
	void linearRowScale(int row, double scale);

	void zeroRow(int row);

	Matrix* getRow(int row);
	Matrix* getCol(int col);

	void setRow(int row, double* rowVector);

	bool rowExists(double* rowVector);
	bool linearCombinationExists(double* rowVector);

	void addRow();

	Matrix* RREF();

	Matrix* operator*(Matrix& rhs);

	bool addLIRow(int row, double* rowVector);
	
};