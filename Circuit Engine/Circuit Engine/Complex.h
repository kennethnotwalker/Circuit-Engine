#pragma once

#include <iostream>
class complex {
public:
    double real;
    double imaginary;

    complex(double re, double im)
    {
        real = re;
        imaginary = im;
    }

    complex(double re)
    {
        real = re;
        imaginary = 0;
    }

    complex(float re)
    {
        real = re;
        imaginary = 0;
    }

    complex(int re)
    {
        real = re;
        imaginary = 0;
    }



    friend std::ostream& operator<< (std::ostream& os, const complex& x)
    {
        os << "(" << x.real << " + " << x.imaginary << "i)";
        return os;
    }


};

std::string str(complex m);

complex operator+(complex a, complex b);

bool operator==(complex a, complex b);

complex operator*(complex a, complex b);

complex operator/(complex A, complex B);

complex operator-(complex a, complex b);

double complex_modulus(complex a);
double arg(complex a);
complex exp(complex a);
complex ln(complex a);
complex operator^(complex x, complex y);
complex sqrt(complex a);
