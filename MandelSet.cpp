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
	render_engine = new RenderThreads<MandelSet>(4, this, CreateImgPart);
	
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


