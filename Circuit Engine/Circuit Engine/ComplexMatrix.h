#pragma once

#include <cstdarg>
#include <vector>
#include "MathVector.h"
#include "Complex.h"

class ComplexMatrix
{
private:
	complex** data;
public:
	int rows, cols = 0;
	ComplexMatrix(int _rows, int _columns);
	ComplexMatrix(ComplexMatrix* toCopy);
	~ComplexMatrix();

	void print();

	void map(vector<vector<complex>> vec);

	void insert(int row, int col, complex value);
	void addTo(int row, int col, complex value);
	complex get(int row, int col);

	void linearRowOperation(int sourceRow, int targetRow, complex scale);
	void linearRowScale(int row, complex scale);

	void zeroRow(int row);

	ComplexMatrix* getRow(int row);
	ComplexMatrix* getCol(int col);

	void setRow(int row, complex* rowVector);

	bool rowExists(complex* rowVector);
	bool linearCombinationExists(complex* rowVector);

	void addRow();

	ComplexMatrix* RREF();

	ComplexMatrix* operator*(ComplexMatrix& rhs);

	bool addLIRow(int row, complex* rowVector);

};