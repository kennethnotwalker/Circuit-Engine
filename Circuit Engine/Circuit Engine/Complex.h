#pragma once

#include <iostream>
class complex {
public:
    double real;
    double imaginary;

    complex()
    {
        real = 0;
        imaginary = 0;
    };

    complex(double re, double im)
    {
        real = re;
        imaginary = im;
    };

    complex(double re)
    {
        real = re;
        imaginary = 0;
    };

    complex(float re)
    {
        real = re;
        imaginary = 0;
    };

    complex(int re)
    {
        real = re;
        imaginary = 0;
    };



    friend std::ostream& operator<< (std::ostream& os, const complex& x)
    {
        if (x.imaginary != 0)
        {
            os << x.real << " + " << x.imaginary << "i";
        }
        else if(x.imaginary == 0)
        {
            os << x.real;
        }
        else
        {
            os << x.imaginary << "i";
        }
        
        return os;
    }

    complex operator-() const { return complex(-real, -imaginary); }
    complex operator+() const { return complex(real, imaginary); }

};

std::string str(complex m);



bool operator==(complex a, complex b);
bool operator!=(complex a, complex b);

complex operator+(complex a, complex b);
complex operator*(complex a, complex b);
complex operator/(complex A, complex B);
complex operator-(complex a, complex b);

void operator+=(complex& a, complex b);
void operator*=(complex& a, complex b);
void operator/=(complex& A, complex B);
void operator-=(complex& a, complex b);

double abs(complex a);
double complex_modulus(complex a);
double arg(complex a);
complex exp(complex a);
complex ln(complex a);
complex operator^(complex x, complex y);
complex sqrt(complex a);
