#pragma once
#include <SFML/Graphics.hpp>
#include <complex>
#include <cmath>


namespace Mandelbrot {

	struct MandelbrotSet {
		sf::Image	img;
		sf::Texture tex;
		sf::Sprite	sp;
		std::mutex *thread_lockers;
		std::mutex *img_lockers;
		std::mutex *wait;
		bool *is_started;

		MandelbrotSet(sf::Vector2u img_size, sf::Vector2f sp_position, unsigned number_of_threads) {

			img.create(img_size.x, img_size.y, sf::Color::Black);
			sp.setPosition(sp_position);
			sp.setOrigin(float(img_size.x >> 1),
						float(img_size.y >> 1));

			thread_lockers =	new std::mutex[number_of_threads];
			img_lockers =		new std::mutex[number_of_threads];
			wait =				new std::mutex[number_of_threads];
			is_started = new bool[number_of_threads];
			
			
				
			for (unsigned i = 0; i < number_of_threads; i++)
			{
				thread_lockers[i].lock();
				is_started[i] = false;
			}
			
		}
		~MandelbrotSet() {
			delete[] thread_lockers;
			delete[] img_lockers;
			delete[] is_started;
			delete[] wait;
		}

	};


	void isInMandelbrotSet( std::complex <double> z,
								std::complex <double> *c,
								double		&radius,
								unsigned	&max_iter,
								unsigned	*current_iter) {
		if (abs(z) < radius && *current_iter < max_iter) {
			*current_iter += 1;
			isInMandelbrotSet(((z*z) + *c), c, radius, max_iter, current_iter);
		}
	}


	std::complex <double> findComplex(unsigned x, unsigned y, double scale, double mx, double my) {
		return std::complex <double>(	(double)x * scale * 1.0f / 200.0f + mx,
										(double)y * scale * 1.0f / 200.0f + my);

	}

	double countMx_My(unsigned left, unsigned right, double scale, double delta) {
		return ((double)(left)* scale * 1.0f / 200.0f - (double)(right)* scale * 1.0f / 200.0f) / 2.0f + delta;
	}
	double countDeltaMx_My(int last, int current, double scale) {
		return (double)(last)* scale * 1.0f / 200.0f - (double)(current)* scale * 1.0f / 200.0f;
	}


	void createImgPart(	sf::Image		&img, 
						sf::Vector2u	&render_pos, 
						sf::Vector2u	&render_size, 
						double			&scale, 
						double			mx, 
						double			my, 
						unsigned		&max_iter, 
						double			&radius,
						sf::Color		&color) {
		

		unsigned	iteration = 0,
					max_x = render_pos.x + render_size.x,
					max_y = render_pos.y + render_size.y,
					x = render_pos.x,
					y;

		for (x; x < max_x; x++) {
			y = render_pos.y;
			for (y; y < max_y; y++) {


				isInMandelbrotSet(std::complex <double>(0.0f, 0.0f), &findComplex(x, y, scale, mx, my), radius, max_iter, &iteration);


				
				img.setPixel(x, y, sf::Color((color.r * iteration) % 256,
											(color.g * iteration) % 256,
											(color.b * iteration) % 256,
											color.a));
				


				iteration = 0;
			}
		}
		
	}
	

}