#include "Complex.h"


Complex::Complex()
{
}


Complex::Complex(double r, double i)
{
	this->r = r; this->i = i;
}





double Complex::abs()
{
	return (this->r * this->r + this->i * this->i);
}


Complex Complex::operator+(const Complex &c)
{
	return Complex(this->r + c.r, this->i + c.i);
}


Complex Complex::operator*(const Complex &c)
{
	return Complex(this->r * c.r - this->i * c.i, 2 * this->r * c.i);
}






Complex::~Complex()
{
}
