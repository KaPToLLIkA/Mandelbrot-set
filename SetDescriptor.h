#pragma once
#include <mutex>

namespace dscr {

	// BASIC ELEMENTS SECTION
	template <typename T> 
	struct Vec2
	{
		T x, y;
	};

	enum class ID
	{
		Mandelbrot

	};

	class basic_descriptor
	{
	protected:
		int setID;
	public:
		int getSetID() { return setID; }
		virtual ~basic_descriptor() {}
	};
	// END OF BASIC ELEMENTS SECTION

	


	// MANDELBROT_SET SECTION
	class Mandelbrot_dscr final : public basic_descriptor
	{
	public:
		std::mutex changing;
		std::mutex lock_next_xy;

		Vec2<unsigned> img_size;
		Vec2<float> img_pos;

		double	scale = 1.f,
				radius = 8.f,
				delta_mx = 0.f,
				delta_my = 0.f,
				mx = 0.f,
				my = 0.f;


		unsigned	number_of_color_points = 4,
					number_of_iterations = number_of_color_points * 12,
					number_of_render_threads = 1,
					next_x = 0,
					next_y = 0;

		Mandelbrot_dscr() {}
		~Mandelbrot_dscr() {}
	};
	// END OF MANDELBROT_SET SECTION

} //end of dscr namespace