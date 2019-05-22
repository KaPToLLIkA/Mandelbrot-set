#include "Color.h"

namespace color {

	ColorInt operator+(const ColorInt &l, const ColorInt &r)
	{
		return ColorInt(l.r + r.r, l.g + r.g, l.b + r.b, l.a + r.a);
	}

	ColorInt operator-(const ColorInt &l, const ColorInt &r)
	{
		return ColorInt(l.r - r.r, l.g - r.g, l.b - r.b, l.a - r.a);
	}

	ColorInt operator*(const ColorInt &arg, uint8_t koefficient)
	{
		return ColorInt(arg.r * koefficient, arg.g * koefficient, arg.b * koefficient, arg.a * koefficient);
	}

	const ColorInt & ColorInt::operator+=(const ColorInt &arg)
	{
		if (this == &arg) return *this;

		this->r += arg.r;
		this->g += arg.g;
		this->b += arg.b;
		this->a += arg.a;
		return *this;
	}

	const ColorInt & ColorInt::operator-=(const ColorInt &arg)
	{
		if (this == &arg) return *this;


		this->r -= arg.r;
		this->g -= arg.g;
		this->b -= arg.b;
		this->a -= arg.a;
		return *this;
	}

	const ColorInt & ColorInt::operator=(const ColorInt &arg)
	{
		if (this == &arg) return *this;
		this->r = arg.r;
		this->g = arg.g;
		this->b = arg.b;
		this->a = arg.a;

		return *this;
	}

} //end of color namespace