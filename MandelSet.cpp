#include "MandelSet.h"


void isInMandelbrotSet(std::complex <double> z,
						std::complex <double> *c,
						double				  &radius,
						unsigned		      max_iter,
						unsigned			  *current_iter)
{
	if (abs(z) < radius && *current_iter < max_iter) {
		*current_iter += 1;
		isInMandelbrotSet(((z*z) + *c), c, radius, max_iter, current_iter);
	}
}

std::complex <double> findComplex(unsigned x,
								unsigned y,
								double   scale,
								double   mx,
								double   my)
{
	return std::complex <double>((double)x * scale * 1.0f / 200.0f + mx,
		(double)y * scale * 1.0f / 200.0f + my);

}

double countMx_My(	unsigned left,
					unsigned right,
					double   scale,
					double   delta)
{
	return ((double)(left)* scale * 1.0f / 200.0f - (double)(right)* scale * 1.0f / 200.0f) / 2.0f + delta;
}


double countDeltaMx_My(	int    last,
						int    current,
						double scale)
{
	return (double)(last)* scale * 1.0f / 200.0f - (double)(current)* scale * 1.0f / 200.0f;
}


void CreateImgPart(MandelSet* _this, 
					std::mutex * pauser,
					std::mutex * work_locker,
					std::mutex * wait_restart,
					std::condition_variable * alert,
					bool * notified,
					bool * exit,
					bool * stop_render)
{
	unsigned x = 0;
	unsigned y = 0;
	unsigned iteration = 0;

	
	while (!*exit)
	{
		std::unique_lock <std::mutex> locker(*wait_restart);

		while (!*notified) alert->wait(locker);
		*notified = false;

		work_locker->lock();
		while (x < _this->dscr.img_size.x - 1 && !*stop_render) 
		{
			pauser->lock();
			pauser->unlock();

			_this->dscr.lock_next_xy.lock();
			y = _this->dscr.next_y;
			x = _this->dscr.next_x;

			++_this->dscr.next_y;

			if (_this->dscr.next_y == _this->dscr.img_size.y)
			{

				++_this->dscr.next_x;
				_this->dscr.next_y = 0;

			}
			_this->dscr.lock_next_xy.unlock();


			isInMandelbrotSet(std::complex <double>(0.0f, 0.0f),
				&findComplex(x, y, _this->dscr.scale, _this->dscr.mx, _this->dscr.my), _this->dscr.radius, _this->dscr.number_of_iterations, &iteration);


			_this->iter_matrix[x][y] = --iteration;

			iteration = 0;

		}
		work_locker->unlock();
	}
}


MandelSet::MandelSet(Vec2<float> img_pos, 
					 Vec2<unsigned> img_size)
{
	dscr.img_size = img_size;
	dscr.img_pos = img_pos;
	render_engine = new RenderThreads(4, this, CreateImgPart);
	
	iter_matrix = new unsigned*[img_size.x];
	for (size_t i = 0; i < img_size.x; ++i)
	{
		iter_matrix[i] = new unsigned[img_size.y];
	}

}

MandelSet::~MandelSet()
{
	for (size_t i = 0; i < dscr.img_size.x; ++i)
		delete[] iter_matrix[i];

	delete[] iter_matrix;
}

void MandelSet::createGradientColorsTable()
{
	color_table.clear();
	color_table.resize(dscr.number_of_iterations);

	float delta_iter = static_cast<float>(color_table.size()) / 6.0f;

	size_t max_iter1 = static_cast<size_t>(round(delta_iter * 1.0f));
	size_t max_iter2 = static_cast<size_t>(round(delta_iter * 2.0f));
	size_t max_iter3 = static_cast<size_t>(round(delta_iter * 3.0f));
	size_t max_iter4 = static_cast<size_t>(round(delta_iter * 4.0f));
	size_t max_iter5 = static_cast<size_t>(round(delta_iter * 5.0f));

	color::ColorFloat delta(0.0f, 0.0f, 1.0f/delta_iter, 0.0f);

	// r = const, b++
	color_table[0] = { 1.f, 0.f, 0.f, 1.f };

	for (size_t i = 1; i < max_iter1; ++i)
		color_table[i] = color_table[i-1] + delta;


	// b = const, r--
	delta = { 1.f / delta_iter, 0.f, 0.f, 0.f };
	color_table[max_iter1] = { 1.f, 0.f, 1.f, 1.f };

	for (size_t i = max_iter1 + 1; i < max_iter2; ++i)
		color_table[i] = color_table[i - 1] - delta;

	// b = const, g++
	delta = { 0.f, 1.f/delta_iter, 0.f, 0.f };
	color_table[max_iter2] = { 0.f, 0.f, 1.f, 1.f };

	for (size_t i = max_iter2 + 1; i < max_iter3; ++i)
		color_table[i] = color_table[i - 1] + delta;

	// g = const, b--
	delta = { 0.f, 0.f, 1.f / delta_iter, 0.f };
	color_table[max_iter3] = { 0.f, 1.f, 1.f, 1.f };

	for (size_t i = max_iter3 + 1; i < max_iter4; ++i)
		color_table[i] = color_table[i - 1] - delta;

	// g = const, r++
	delta = { 1.f / delta_iter, 0.f, 0.f, 0.f };
	color_table[max_iter4] = { 0.f, 1.f, 0.f, 1.f };

	for (size_t i = max_iter4 + 1; i < max_iter5; ++i)
		color_table[i] = color_table[i - 1] + delta;

	// r = const, g--
	delta = { 0.f, 1.f / delta_iter,  0.f, 0.f };
	color_table[max_iter5] = { 1.f, 1.f, 0.f, 1.f };

	for (size_t i = max_iter5 + 1; i < color_table.size(); ++i)
		color_table[i] = color_table[i - 1] - delta;

}


