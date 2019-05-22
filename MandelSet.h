#pragma once
#include <complex>
#include "Color.h"
#include "SetDescriptor.h"
#include "RenderThreads.h"

class MandelSet: public RenderThreads<MandelSet>
{
private:
	dscr::Mandelbrot_dscr dscr;

	unsigned **iter_matrix;


public:
	std::mutex lock_matrix;
	
	
	MandelSet& operator=(const MandelSet&) = delete;
	MandelSet(const MandelSet&) = delete;
	MandelSet() = delete;


	MandelSet(dscr::Vec2<float> img_pos, dscr::Vec2<unsigned> img_size);
	~MandelSet();




	friend void CreateImgPart(MandelSet* _this, std::mutex *locker, bool *exit);

};

