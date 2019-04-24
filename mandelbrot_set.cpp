#include "mandelbrot_set.h"


namespace Mandelbrot
{
	//class methods
	MandelbrotSet::MandelbrotSet(sf::Vector2u img_size,
								sf::Vector2f sp_position,
								unsigned number_of_threads,
								unsigned number_of_color_points)
	{
		lock_stream_is_running = new std::mutex[number_of_threads];
		restart_render.resize(number_of_threads);
		for (unsigned i = 0; i < number_of_threads; ++i)
			restart_render[i] = true;



		img.create(img_size.x, img_size.y, sf::Color::Black);
		sp.setPosition(sp_position);
		sp.setOrigin(float(img_size.x >> 1),
			float(img_size.y >> 1));



		unsigned delta = 256 / number_of_color_points;
		for (unsigned i = 0; i < number_of_color_points; ++i)
		{
			sf::Uint8 color = (sf::Uint8)(delta * i);
			color_point.push_back({
					color,
					color,
					color,
					(sf::Uint8)255
				});
			

		}




		this->koeffs = new unsigned*[img_size.x];
		for (unsigned i = 0; i < img_size.x; ++i)
			this->koeffs[i] = new unsigned[img_size.y];

#ifdef ENABLE_LOGGING
		log.open(log_name, std::ios::trunc);
#endif

	}
	MandelbrotSet::~MandelbrotSet()
	{
		for (size_t i = 0; i < img.getSize().x; ++i)
			delete[] koeffs[i];

		delete[] lock_stream_is_running;
		delete[] koeffs;

#ifdef ENABLE_LOGGING
		log.close();
#endif
	}

	void MandelbrotSet::createColorsTable(unsigned max_iter)
	{
#ifdef ENABLE_LOGGING
		lock_log_print.lock();
		if (log.is_open())
			log << "[INFO] colors creating: thread id {"
			<< std::this_thread::get_id() << "}\n";
		lock_log_print.unlock();
#endif
		
		
		if (last_max_iter != max_iter || update_colors)
		{
			
#ifdef ENABLE_LOGGING
			lock_log_print.lock();
			if (log.is_open())
				log << "[INFO] colors creating: max_iter {" + std::to_string(max_iter) + "}\n";
			lock_log_print.unlock();
#endif
			last_max_iter = max_iter;
			float delta_colors = (float)max_iter / color_point.size();
			
			
			color_table.clear();
			color_table.resize(max_iter);


			unsigned idx_prev = 0;
			unsigned idx_next = 0;
			float delta_r;
			float delta_g;
			float delta_b;
			float delta_a;
			
			color_table[idx_prev] = color_point[0];
#ifdef ENABLE_LOGGING
			lock_log_print.lock();
			if (log.is_open())
				log << "[INFO] colors creating: colors table resized\n";
			lock_log_print.unlock();
#endif
			for (size_t i = 1; i < color_point.size(); ++i)
			{
				
				idx_next = idx_prev + (unsigned)round(delta_colors);
				color_table[idx_next] = color_point[i];
				
				delta_r = ((float)color_table[idx_prev].r - (float)color_table[idx_next].r) / delta_colors;
				delta_g = ((float)color_table[idx_prev].g - (float)color_table[idx_next].g) / delta_colors;
				delta_b = ((float)color_table[idx_prev].b - (float)color_table[idx_next].b) / delta_colors;
				delta_a = ((float)color_table[idx_prev].a - (float)color_table[idx_next].a) / delta_colors;
				
				

				for (int x = 1; x < delta_colors; x++)
				{
					
					color_table[idx_prev + (size_t)x] = {
						sf::Uint8(color_table[idx_prev].r - round(delta_r * x)),
						sf::Uint8(color_table[idx_prev].g - round(delta_g * x)),
						sf::Uint8(color_table[idx_prev].b - round(delta_b * x)),
						sf::Uint8(color_table[idx_prev].a - round(delta_a * x))

					};
					

				}
			
				idx_prev = idx_next;
			}
			
			delta_r = ((float)color_table[idx_prev].r - (float)color_table[0].r) / delta_colors;
			delta_g = ((float)color_table[idx_prev].g - (float)color_table[0].g) / delta_colors;
			delta_b = ((float)color_table[idx_prev].b - (float)color_table[0].b) / delta_colors;
			delta_a = ((float)color_table[idx_prev].a - (float)color_table[0].a) / delta_colors;

			for (int x = 1; x < delta_colors; x++)
			{
				
				color_table[idx_prev + (size_t)x] = {
					sf::Uint8(color_table[idx_prev].r - round(delta_r * x)),
					sf::Uint8(color_table[idx_prev].g - round(delta_g * x)),
					sf::Uint8(color_table[idx_prev].b - round(delta_b * x)),
					sf::Uint8(color_table[idx_prev].a - round(delta_a * x))

				};

			}
#ifdef ENABLE_LOGGING
			lock_log_print.lock();
			if (log.is_open())
				log << "[INFO] colors creating: all colors calculated\n";
			lock_log_print.unlock();
#endif
			update_colors = false;
		}
		

#ifdef ENABLE_LOGGING
		lock_log_print.lock();
		if (log.is_open())
			log << "[INFO] colors creating: ending\n";
		lock_log_print.unlock();
#endif
	}

	void MandelbrotSet::updatePixels(	unsigned	 &next_y,
										unsigned	 &next_x,
										sf::Vector2u &img_size)
	{
		unsigned x = 0;
		unsigned y = 0;
		
#ifdef ENABLE_LOGGING
		lock_log_print.lock();
		if (log.is_open())
			log << "UPDATING ID[" << std::this_thread::get_id() << "] starting: input{x: " + std::to_string(next_x) + "; y:" + std::to_string(next_y) + "}\n";
		lock_log_print.unlock();
#endif

		while (x < img_size.x) {
			lock_point_procesing.lock();
			y = next_y;
			x = next_x;
			++next_y;

			if (next_y == img_size.y)
			{
				++next_x;
				next_y = 0;
#ifdef ENABLE_LOGGING
				lock_log_print.lock();
				if (log.is_open())
					log << "UPDATING ID[" << std::this_thread::get_id() << "]: current"
					"{x: " + std::to_string(x) + "; y:" + std::to_string(y) + "}\n";
				lock_log_print.unlock();
#endif
			}
			lock_point_procesing.unlock();

			if (main_window_closed || stop_render || x == img_size.x)
			{
#ifdef ENABLE_LOGGING
				lock_log_print.lock();
				if (log.is_open())
					log << "UPDATING ID[" << std::this_thread::get_id() << "] stopped: current"
					"{x: " + std::to_string(x) + "; stop_rendering_flag:" + std::to_string(stop_render) + "}\n";
				lock_log_print.unlock();
#endif
				break;
			}
			
			this->lock_img.lock();
			this->img.setPixel(x, y, color_table[this->koeffs[x][y]]);
			this->lock_img.unlock();
			
		}
		
#ifdef ENABLE_LOGGING
		lock_log_print.lock();
		if (log.is_open())
			log << "UPDATING ID[" << std::this_thread::get_id() << "] stopped <<===========================>>\n";
		lock_log_print.unlock();
#endif
	}

	void MandelbrotSet::createImgPart(	unsigned	  id,
										unsigned	 &next_y,
										unsigned	 &next_x,
										sf::Vector2u &img_size,
										double		  scale,
										double		  mx,
										double		  my,
										unsigned	  max_iter,
										double		 &radius)
	{


		unsigned x = 0;
		unsigned y = 0;
		unsigned iteration = 0;
#ifdef ENABLE_LOGGING
		lock_log_print.lock();
		if (log.is_open())
			log << "RENDER ID[" << std::this_thread::get_id() << "] starting:"
			" input{x: " + std::to_string(next_x) + "; y:" + std::to_string(next_y) + "}\n"
			" ID:     " + std::to_string(id) +      "\n"
			" SCALE:  " + std::to_string(scale) +   "\n"
			" MX:     " + std::to_string(mx) +      "\n"
			" MY:     " + std::to_string(my) +      "\n"
			" ITER:   " + std::to_string(max_iter) +"\n";
		lock_log_print.unlock();
#endif

		
		while(x < img_size.x) {
			lock_point_procesing.lock();
			y = next_y;
			x = next_x;
			++next_y;

			if (next_y == img_size.y)
			{
				
				++next_x;
				next_y = 0;
#ifdef ENABLE_LOGGING
				lock_log_print.lock();
				if (log.is_open())
					log << "RENDER ID[" << std::this_thread::get_id() << "]: current"
					"{x: " + std::to_string(x) + "; y:" + std::to_string(y) + "}\n";
				lock_log_print.unlock();
#endif
			}
			lock_point_procesing.unlock();


			if (main_window_closed || stop_render || x == img_size.x)
			{
#ifdef ENABLE_LOGGING
				lock_log_print.lock();
				if (log.is_open())
					log << "RENDER ID[" << std::this_thread::get_id() << "] stopped: current"
					"{x: " + std::to_string(x) + "; stop_rendering_flag:" + std::to_string(stop_render) + "}\n";
				lock_log_print.unlock();
#endif
				
				break;
			}


			isInMandelbrotSet(std::complex <double>(0.0f, 0.0f), &findComplex(x, y, scale, mx, my), radius, max_iter, &iteration);
			lock_img.lock();
			--iteration;
			this->img.setPixel(x, y, color_table[iteration]);
			try {
				this->koeffs[x][y] = iteration;
			}
			catch (...)
			{

				
				lock_img.unlock();
				break;
			}
			lock_img.unlock();
			iteration = 0;
		}
#ifdef ENABLE_LOGGING
		lock_log_print.lock();
		if (log.is_open())
			log << "RENDER ID[" << std::this_thread::get_id() << "] stopped <<===========================>>\n";
		lock_log_print.unlock();
#endif
		
	}

	void MandelbrotSet::restartRender()
	{
#ifdef ENABLE_LOGGING
		lock_log_print.lock();
		if (log.is_open())
			log << "RESTARTING THREADS ID[" << std::this_thread::get_id() << "] \n";
		lock_log_print.unlock();
#endif
		
		std::unique_lock <std::mutex> locker(lock_render);
		for (size_t i = 0; i < restart_render.size(); ++i)
		{
			lock_stream_is_running[i].lock();
		}
		
		for (size_t i = 0; i < restart_render.size(); ++i)
		{
			restart_render[i] = true;
		}
		lock_render_condition.notify_all();
		
		for (size_t i = 0; i < restart_render.size(); ++i)
		{
			lock_stream_is_running[i].unlock();
		}
		
#ifdef ENABLE_LOGGING
		lock_log_print.lock();
		if (log.is_open())
			log << "RESTARTING THREADS ID[" << std::this_thread::get_id() << "] restarted\n";
		lock_log_print.unlock();
#endif
	}

	void MandelbrotSet::saveColors(std::string file_name)
	{
		std::time_t cur_time = std::time(nullptr);
		std::string time_str = std::ctime(&cur_time);

		for (size_t i = 0; i < time_str.size(); ++i)
			if (time_str[i] == ':') time_str[i] = '.';

		time_str.erase(--time_str.end());
		std::ofstream out(".\\colors\\" + file_name + " " + time_str + ".colset", std::ios::trunc);
		out << color_point.size() << std::endl;
		for (size_t i = 0; i < color_point.size(); ++i)
		{
			out << color_point[i].r << std::endl;
			out << color_point[i].g << std::endl;
			out << color_point[i].b << std::endl;
			out << color_point[i].a << std::endl;
		}


		out.close();

	}

	void MandelbrotSet::loadColors(std::string file_name)
	{
		std::stringstream conv;
		std::string buffer;
		std::ifstream in(file_name);
		if (in.is_open())
		{
			size_t size = 0;
			std::getline(in, buffer, '\n');
			conv << buffer;
			conv >> size;
			conv.clear();
			color_point.clear();
			color_point.resize(size);
			for (size_t i = 0; i < size; ++i)
			{
				std::getline(in, buffer, '\n');
				conv << buffer;
				conv >> color_point[i].r;
				conv.clear();
				
				std::getline(in, buffer, '\n');
				conv << buffer;
				conv >> color_point[i].g;
				conv.clear();
			
				std::getline(in, buffer, '\n');
				conv << buffer;
				conv >> color_point[i].b;
				conv.clear();
				
				std::getline(in, buffer, '\n');
				conv << buffer;
				conv >> color_point[i].a;
				conv.clear();
			
			}


		}
		in.close();
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






	









}