#include "Color.h"

namespace color {

	ColorFloat operator+(const ColorFloat &l, const ColorFloat &r)
	{
		
		return ColorFloat(
			l.r + r.r > 1.0f ? 1.0f : l.r + r.r,
			l.g + r.g > 1.0f ? 1.0f : l.g + r.g,
			l.b + r.b > 1.0f ? 1.0f : l.b + r.b,
			l.a + r.a > 1.0f ? 1.0f : l.a + r.a
		);
	}

	ColorFloat operator-(const ColorFloat &l, const ColorFloat &r)
	{
		return ColorFloat(
			l.r - r.r < 0.0f ? 0.0f : l.r - r.r,
			l.g - r.g < 0.0f ? 0.0f : l.g - r.g,
			l.b - r.b < 0.0f ? 0.0f : l.b - r.b,
			l.a - r.a < 0.0f ? 0.0f : l.a - r.a
		);
	}

	ColorFloat operator*(const ColorFloat &l, float k)
	{
		ColorFloat x(l.r * k, l.g * k, l.b * k, l.a * k);
		
		x.r = ((x.r > 1.0f ? 1.0 : x.r) < 0.0f ? 0.0f : x.r);
		x.g = ((x.g > 1.0f ? 1.0 : x.g) < 0.0f ? 0.0f : x.g);
		x.b = ((x.b > 1.0f ? 1.0 : x.b) < 0.0f ? 0.0f : x.b);
		x.a = ((x.a > 1.0f ? 1.0 : x.a) < 0.0f ? 0.0f : x.a);

		return x;
	}

	const ColorFloat & ColorFloat::operator+=(const ColorFloat &arg)
	{
		this->r += arg.r;
		this->g += arg.g;
		this->b += arg.b;
		this->a += arg.a;

		this->r = this->r > 1.0f ? 1.0f : this->r;
		this->g = this->g > 1.0f ? 1.0f : this->g;
		this->b = this->b > 1.0f ? 1.0f : this->b;
		this->a = this->a > 1.0f ? 1.0f : this->a;

		return *this;
	}

	const ColorFloat & ColorFloat::operator-=(const ColorFloat &arg)
	{
		this->r -= arg.r;
		this->g -= arg.g;
		this->b -= arg.b;
		this->a -= arg.a;

		this->r = this->r < 0.0f ? 0.0f : this->r;
		this->g = this->g < 0.0f ? 0.0f : this->g;
		this->b = this->b < 0.0f ? 0.0f : this->b;
		this->a = this->a < 0.0f ? 0.0f : this->a;

		return *this;
	}

	const ColorFloat & ColorFloat::operator=(const ColorFloat &arg)
	{
		this->r = arg.r;
		this->g = arg.g;
		this->b = arg.b;
		this->a = arg.a;
		
		return *this;
	}

} //end of color namespace