#pragma once
#include <stdint.h>

namespace color {
	class ColorInt
	{
	private:
		int8_t r;
		int8_t g;
		int8_t b;
		int8_t a;
	public:



		ColorInt(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) :
			r(r - 128), g(g - 128), b(b - 128), a(a - 128)
		{

		}

		ColorInt() : r(-128), g(-128), b(-128), a(127)
		{

		}

		void setR(uint8_t val) { r = val - 128; }
		void setG(uint8_t val) { g = val - 128; }
		void setB(uint8_t val) { b = val - 128; }
		void setA(uint8_t val) { a = val - 128; }

		uint8_t R() { return uint8_t(r + 128); }
		uint8_t G() { return uint8_t(g + 128); }
		uint8_t B() { return uint8_t(b + 128); }
		uint8_t A() { return uint8_t(a + 128); }

		friend ColorInt operator+(const ColorInt&, const ColorInt&);
		friend ColorInt operator-(const ColorInt&, const ColorInt&);
		friend ColorInt operator*(const ColorInt&, uint8_t);

		const ColorInt& operator+=(const ColorInt&);
		const ColorInt& operator-=(const ColorInt&);

		const ColorInt& operator=(const ColorInt&);

		virtual ~ColorInt()
		{

		}
	};

	class ColorUint
	{
	private:
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	public:
		ColorUint(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) :
			r(r), g(g), b(b), a(a)
		{

		}

		ColorUint() : r(0), g(0), b(0), a(255)
		{

		}


		uint8_t R() { return r; }
		uint8_t G() { return g; }
		uint8_t B() { return b; }
		uint8_t A() { return a; }

		virtual ~ColorUint()
		{

		}
	};


	class ColorFloat
	{


	};
} //end of color namespace

