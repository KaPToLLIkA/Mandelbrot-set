#pragma once
#include <complex>
#include "Color.h"
#include "SetDescriptor.h"
#include "RenderThreads.h"
#include "typedef.h"

#define FRAME_MANDEL_X 800
#define FRAME_MANDEL_Y 600

class MandelSet
{
private:
	dscr::Mandelbrot_dscr dscr;

	unsigned **iter_matrix;


public:
	std::mutex lock_matrix;
	RenderThreads<MandelSet> *render_engine;
	
	MandelSet& operator=(const MandelSet&) = delete;
	MandelSet(const MandelSet&) = delete;
	MandelSet() = delete;


	MandelSet(Vec2<float> img_pos, Vec2<unsigned> img_size);
	~MandelSet();




	friend void CreateImgPart(MandelSet* _this, 
								std::mutex * pauser,
								std::mutex * work_locker,
								std::mutex * wait_restart,
								std::condition_variable * alert,
								bool * notified,
								bool * exit,
								bool * stop_render);

};

