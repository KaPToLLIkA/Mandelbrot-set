#ifndef _COMPLEX_
#define _COMPLEX_



class Complex
{
private:
	double r, i;

public:
	Complex();
	Complex(double r, double i);


	double abs();
	Complex operator+(const Complex &c);
	Complex operator*(const Complex &c);
	

	~Complex();
};




#endif // !_COMPLEX_