#pragma once
#include <complex>
#include "SFML/System/Vector2.hpp"
#include "Color.h"
#include "SetDescriptor.h"
#include "RenderThreads.h"


#define FRAME_MANDEL_X 800
#define FRAME_MANDEL_Y 600

class MandelSet
{
private:
	dscr::Mandelbrot_dscr dscr;
	
	unsigned **iter_matrix;

	std::vector <color::ColorFloat> color_table;
public:
	std::mutex lock_matrix;
	RenderThreads *render_engine;
	
	MandelSet& operator=(const MandelSet&) = delete;
	MandelSet(const MandelSet&) = delete;
	MandelSet() = delete;


	MandelSet(sf::Vector2f img_pos, sf::Vector2u img_size, size_t number_of_threads);
	~MandelSet();




	friend void CreateImgPartMandelbrot(MandelSet* _this,
										RenderThreads *thr,
										size_t idx);

	void createGradientColorsTable();

};

