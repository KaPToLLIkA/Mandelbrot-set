#include "mandelbrot_set.h"


namespace Mandelbrot
{
	//class methods
	MandelbrotSet::MandelbrotSet(sf::Vector2u img_size,
		sf::Vector2f sp_position,
		unsigned number_of_threads,
		unsigned number_of_color_points)
	{

		img.create(img_size.x, img_size.y, sf::Color::Black);
		sp.setPosition(sp_position);
		sp.setOrigin(float(img_size.x >> 1),
			float(img_size.y >> 1));

		thread_lockers = new std::mutex[number_of_threads];
		img_lockers = new std::mutex[number_of_threads];
		wait = new std::mutex[number_of_threads];
		is_started = new bool[number_of_threads];



		for (unsigned i = 0; i < number_of_threads; i++)
		{
			thread_lockers[i].lock();
			is_started[i] = false;
		}

		for (unsigned i = 0; i < number_of_color_points; ++i)
			color_point.push_back({ 0, 0, 0, 255 });


	}
	MandelbrotSet::~MandelbrotSet() {
		delete[] thread_lockers;
		delete[] img_lockers;
		delete[] is_started;
		delete[] wait;
	}

	void MandelbrotSet::createColorsTable(unsigned max_iter)
	{
	
		if (last_max_iter != max_iter)
		{
			last_max_iter = max_iter;
			int delta_colors = max_iter / color_point.size();
			
			
			color_table.clear();
			color_table.resize(max_iter);


			unsigned idx_prev = 0;
			unsigned idx_next = 0;
			int delta_r;
			int delta_g;
			int delta_b;
			int delta_a;
			
			color_table[idx_prev] = color_point[0];

			for (size_t i = 1; i < color_point.size(); ++i)
			{
				idx_next = idx_prev + delta_colors;
				color_table[idx_next] = color_point[i];
				
				delta_r = ((int)color_table[idx_prev].r - (int)color_table[idx_next].r) / delta_colors;
				delta_g = ((int)color_table[idx_prev].g - (int)color_table[idx_next].g) / delta_colors;
				delta_b = ((int)color_table[idx_prev].b - (int)color_table[idx_next].b) / delta_colors;
				delta_a = ((int)color_table[idx_prev].a - (int)color_table[idx_next].a) / delta_colors;
				
				

				for (int x = 1; x < delta_colors; x++)
				{
					color_table[idx_prev + (size_t)x] = {
						sf::Uint8(color_table[idx_prev].r - delta_r * x),
						sf::Uint8(color_table[idx_prev].g - delta_g * x),
						sf::Uint8(color_table[idx_prev].b - delta_b * x),
						sf::Uint8(color_table[idx_prev].a - delta_a * x),

					};

				}

				idx_prev = idx_next;
			}

			delta_r = ((int)color_table[idx_prev].r - (int)color_table[0].r) / delta_colors;
			delta_g = ((int)color_table[idx_prev].g - (int)color_table[0].g) / delta_colors;
			delta_b = ((int)color_table[idx_prev].b - (int)color_table[0].b) / delta_colors;
			delta_a = ((int)color_table[idx_prev].a - (int)color_table[0].a) / delta_colors;

			for (int x = 1; x < delta_colors; x++)
			{
				color_table[idx_prev + (size_t)x] = {
					sf::Uint8(color_table[idx_prev].r - delta_r * x),
					sf::Uint8(color_table[idx_prev].g - delta_g * x),
					sf::Uint8(color_table[idx_prev].b - delta_b * x),
					sf::Uint8(color_table[idx_prev].a - delta_a * x),

				};

			}


		}
	}

	void MandelbrotSet::updatePixels(int ** koeffs, unsigned im_x, unsigned im_y)
	{
		for (unsigned y = 0; y < im_y; ++y)
		{
			for (unsigned x = 0; x < im_x; ++x)
			{
				this->img.setPixel(x, y, color_table[koeffs[y][x] - 1]);
			}
		}
	}









	//other functions


	void			 isInMandelbrotSet(std::complex <double> z,
		std::complex <double> *c,
		double				  &radius,
		unsigned		      &max_iter,
		unsigned			  *current_iter)
	{
		if (abs(z) < radius && *current_iter < max_iter) {
			*current_iter += 1;
			Mandelbrot::isInMandelbrotSet(((z*z) + *c), c, radius, max_iter, current_iter);
		}
	}






	std::complex <double>findComplex(unsigned x,
		unsigned y,
		double   scale,
		double   mx,
		double   my)
	{
		return std::complex <double>((double)x * scale * 1.0f / 200.0f + mx,
			(double)y * scale * 1.0f / 200.0f + my);

	}









	double		  countMx_My(unsigned left,
		unsigned right,
		double   scale,
		double   delta)
	{
		return ((double)(left)* scale * 1.0f / 200.0f - (double)(right)* scale * 1.0f / 200.0f) / 2.0f + delta;
	}








	double		  countDeltaMx_My(int    last,
		int    current,
		double scale)
	{
		return (double)(last)* scale * 1.0f / 200.0f - (double)(current)* scale * 1.0f / 200.0f;
	}






	void 		          createKoeffsPart(int			 *koeffs[],
		sf::Vector2u &render_pos,
		sf::Vector2u &render_size,
		double		 &scale,
		double		  mx,
		double		  my,
		unsigned	 &max_iter,
		double		 &radius)
	{


		unsigned	iteration = 0,
			max_x = render_pos.x + render_size.x,
			max_y = render_pos.y + render_size.y,
			x = render_pos.x,
			y;

		for (x; x < max_x; x++) {
			y = render_pos.y;
			for (y; y < max_y; y++) {


				isInMandelbrotSet(std::complex <double>(0.0f, 0.0f), &findComplex(x, y, scale, mx, my), radius, max_iter, &iteration);
				koeffs[y][x] = iteration;
				iteration = 0;
			}
		}

	}









}