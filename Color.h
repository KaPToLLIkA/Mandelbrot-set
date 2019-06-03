#pragma once
#include <stdint.h>
#include "SFML/Graphics/Color.hpp"

namespace color {


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
	private:
		float r;
		float g;
		float b;
		float a;
	public:



		ColorFloat(float r, float g, float b, float a = 1.0f) :
			r(((r > 1.0f ? 1.0f : r) < 0.0f ? 0.0f : r)), 
			g(((g > 1.0f ? 1.0f : g) < 0.0f ? 0.0f : g)),
			b(((b > 1.0f ? 1.0f : b) < 0.0f ? 0.0f : b)),
			a(((a > 1.0f ? 1.0f : a) < 0.0f ? 0.0f : a))
		{

		}

		ColorFloat() : r(0.f), g(0.f), b(0.f), a(1.0f)
		{

		}

		void setR(float val) { r = ((val > 1.0f ? 1.0f : val) < 0.0f ? 0.0f : val); }
		void setG(float val) { g = ((val > 1.0f ? 1.0f : val) < 0.0f ? 0.0f : val); }
		void setB(float val) { b = ((val > 1.0f ? 1.0f : val) < 0.0f ? 0.0f : val); }
		void setA(float val) { a = ((val > 1.0f ? 1.0f : val) < 0.0f ? 0.0f : val); }

		float R() { return r; }
		float G() { return g; }
		float B() { return b; }
		float A() { return a; }

		friend ColorFloat operator+(const ColorFloat&, const ColorFloat&);
		friend ColorFloat operator-(const ColorFloat&, const ColorFloat&);
		friend ColorFloat operator*(const ColorFloat&, float);

		const ColorFloat& operator+=(const ColorFloat&);
		const ColorFloat& operator-=(const ColorFloat&);

		const ColorFloat& operator=(const ColorFloat&);

		operator sf::Color();
		

		virtual ~ColorFloat()
		{

		}

	};

} //end of color namespace

