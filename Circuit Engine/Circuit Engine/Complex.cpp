#include "Complex.h"
#include <math.h>
#include <iostream>
#include <string>
#include "Constants.h"

std::string str(complex m) {
    return std::to_string(m.real) + " + " + std::to_string(m.imaginary) + "i";
}



complex operator+(complex a, complex b)
{
    return complex(a.real + b.real, a.imaginary + b.imaginary);
}

bool operator==(complex a, complex b)
{
    return a.real == b.real && a.imaginary == b.imaginary;
}

bool operator!=(complex a, complex b)
{
    return !(a == b);
}

complex operator*(complex a, complex b)
{
    return complex(a.real * b.real - a.imaginary * b.imaginary, a.real * b.imaginary + a.imaginary * b.real);
}

complex operator/(complex A, complex B)
{
    double a = A.real;
    double b = A.imaginary;
    double c = B.real;
    double d = B.imaginary;

    double r = (a * c + b * d) / (c * c + d * d);
    double i = (b * c - a * d) / (c * c + d * d);
    return complex(r, i);
}

complex operator-(complex a, complex b)
{
    return a + b * complex(-1);
}

void operator+=(complex& a, complex b)
{
    a = a + b;
}

void operator-=(complex& a, complex b)
{
    a = a - b;
}

void operator*=(complex& a, complex b)
{
    a = a * b;
}

void operator/=(complex& A, complex B)
{
    A = A/B;
}

double abs(complex a)
{
    return complex_modulus(a);
}

double complex_modulus(complex a)
{
    return sqrt(a.real * a.real + a.imaginary * a.imaginary);
}

double arg(complex a)
{
    return atan2(a.imaginary, a.real);
}

complex exp(complex a)
{
    complex length(exp(a.real));
    complex unit(cos(a.imaginary), sin(a.imaginary));
    return length * unit;
}

complex ln(complex a)
{
    double re = log(complex_modulus(a));
    double im = arg(a);

    return complex(re, im);
}

complex operator^(complex x, complex y)
{
    double a = complex_modulus(x);
    double b = arg(x);
    double scalar = pow(a, y.real) * exp(-b * y.imaginary);
    complex power(0, b * y.real + y.imaginary * log(a));
    complex unit = exp(power);


    return complex(scalar) * unit;
}

complex sqrt(complex a)
{
    return a ^ complex(0.5);
}