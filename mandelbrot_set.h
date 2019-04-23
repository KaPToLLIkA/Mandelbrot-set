#pragma once
#include <SFML/Graphics.hpp>
#include <complex>
#include <cmath>
#include <ctime>
#include <mutex>
#include <fstream>
#include <sstream>

namespace Mandelbrot
{

	struct MandelbrotSet {
		sf::Image	img;
		sf::Texture tex;
		sf::Sprite	sp;

		std::mutex lock_img;
		std::mutex lock_render;
		std::mutex lock_point_procesing;
		std::mutex* lock_stream_is_running;
#ifdef ENABLE_LOGGING
		std::mutex lock_log_print;
		std::ofstream log;
		std::string log_name = "log.txt";
#endif

		std::condition_variable lock_render_condition;
		unsigned last_max_iter = 0;
		unsigned **koeffs;

		bool update_colors = false;
		bool stop_render = false;
		bool update_sprite = true;
		bool main_window_closed = false;
		
		std::vector <bool> restart_render;
		std::vector <sf::Color> color_point;
		std::vector <sf::Color> color_table;
		

		

		MandelbrotSet(sf::Vector2u img_size,
			sf::Vector2f sp_position,
			unsigned number_of_threads,
			unsigned number_of_color_points);
		~MandelbrotSet();



		void createColorsTable(unsigned max_iter);
		void updatePixels(	unsigned	 &next_x,
							unsigned	 &next_y,
							sf::Vector2u &img_size);

		void createImgPart(	unsigned	  id,
							unsigned	 &next_x,
							unsigned	 &next_y,
							sf::Vector2u &img_size,
							double		  scale,
							double		  mx,
							double		  my,
							unsigned	  max_iter,
							double		 &radius);
		void restartRender();
		void saveColors(std::string file_name);
		void loadColors(std::string file_name);

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


	


}