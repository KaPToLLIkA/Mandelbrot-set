#pragma once
#include <mutex>
#include "typedef.h"
#include "SFML/System/Vector2.hpp"

namespace dscr {

	// BASIC ELEMENTS SECTION
	

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
		std::mutex lock_next_xy_colors;

		sf::Vector2u img_size;
		sf::Vector2f img_pos;

		double	scale = 1.f,
				radius = 8.f,
				delta_mx = 0.f,
				delta_my = 0.f,
				mx = 0.f,
				my = 0.f;


		unsigned	number_of_color_points = 4,
					number_of_iterations = 128,
					number_of_render_threads = 1,
					next_x = 0,
					next_y = 0, 
					next_x_colors = 0,
					next_y_colors = 0;

		Mandelbrot_dscr() {}
		~Mandelbrot_dscr() {}
	};
	// END OF MANDELBROT_SET SECTION

} //end of dscr namespace