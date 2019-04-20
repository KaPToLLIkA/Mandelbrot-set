#pragma once
#include <SFML/Graphics.hpp>
#include <complex>
#include <cmath>
#include <mutex>

namespace Mandelbrot
{

	struct MandelbrotSet {
		sf::Image	img;
		sf::Texture tex;
		sf::Sprite	sp;
		std::mutex *thread_lockers;
		std::mutex *img_lockers;
		std::mutex *wait;
		unsigned last_max_iter = 0;

		std::vector <sf::Color> color_point;
		std::vector <sf::Color> color_table;

		bool *is_started;

		MandelbrotSet(sf::Vector2u img_size,
			sf::Vector2f sp_position,
			unsigned number_of_threads,
			unsigned number_of_color_points);
		~MandelbrotSet();



		void createColorsTable(unsigned max_iter);
		void updatePixels(int **koeffs, unsigned im_x, unsigned im_y);
	};


	void isInMandelbrotSet(std::complex <double> z,
		std::complex <double> *c,
		double		&radius,
		unsigned	&max_iter,
		unsigned	*current_iter);


	std::complex <double> findComplex(unsigned x,
		unsigned y,
		double   scale,
		double   mx,
		double   my);

	double countMx_My(unsigned left, unsigned right, double scale, double delta);
	double countDeltaMx_My(int last, int current, double scale);


	void createKoeffsPart(int			 *koeffs[],
		sf::Vector2u &render_pos,
		sf::Vector2u &render_size,
		double		 &scale,
		double		  mx,
		double		  my,
		unsigned	 &max_iter,
		double		 &radius);


}