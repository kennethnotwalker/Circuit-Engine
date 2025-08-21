#include "Util.h"
#include <string>
#include <sstream>
#include <iomanip>
#include "Constants.h"
using namespace std;

string toENotation(double x)
{
	if (abs(x) < ERROR_MARGIN)
	{
		return string("0");
	}
	int e = floor(log10(abs(x)));
	double base = x / pow(10, e);
	stringstream stream;
	stream << std::fixed << setprecision(2) << base;
	return stream.str() + string("*10^") + to_string(e);
}