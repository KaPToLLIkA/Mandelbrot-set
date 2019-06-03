#pragma once
#include <complex>
#include "SFML/System/Vector2.hpp"
#include "Color.h"
#include "SetDescriptor.h"
#include "RenderThreads.h"
#include "typedef.h"


#define FRAME_MANDEL_X          800
#define FRAME_MANDEL_Y          600
#define MANDEL_RENDER_THRDS_NUM 600
#define MANDEL_UPDATE_THRDS_NUM (MANDEL_RENDER_THRDS_NUM >> 1)
#define MANDEL_ZOOM_KOEFF	    1.1f

class MandelSet
{
private:
	dscr::Mandelbrot_dscr dscr;
	
	unsigned **iter_matrix;

	std::mutex	                    lock_matrix;
	std::mutex	                    lock_color_table;
	std::mutex						lock_destroy;
	std::vector <color::ColorFloat> color_table;


	RenderThreads *render_engine;
	RenderThreads *update_colors_engine;


	void ClearIterMatrix();
public:
	Frame          current_frame;

	MandelSet& operator=(const MandelSet&) = delete;
	MandelSet(const MandelSet&)            = delete;
	MandelSet()                            = delete;


	explicit MandelSet(
		sf::Vector2f img_pos, 
		sf::Vector2u img_size, 
		size_t       number_of_render_threads,
		size_t       number_of_update_threads);
	~MandelSet();



	void InitRenderer();
	void DestroyRenderer();
	void RestartRenderer();
	void ZoomIncrease();
	void ZoomDecrease();


	friend void CreateImgPartMandelbrot(
		MandelSet     *_this,
		RenderThreads *thr,
		size_t		   idx);

	friend void UpdateColorMandelbrot(
		MandelSet	   *_this,
		RenderThreads  *thr,
		size_t			idx);


	void createGradientColorsTable();


	static void isInMandelbrotSet(
		std::complex <double> z,
		std::complex <double> *c,
		double				  &radius,
		unsigned		      max_iter,
		unsigned			  *current_iter);



	static std::complex <double> findComplex(
		unsigned x,
		unsigned y,
		double   scale,
		double   mx,
		double   my);



	static double countMx_My(
		unsigned left,
		unsigned right,
		double   scale,
		double   delta);



	static double countDeltaMx_My(
		int    last,
		int    current,
		double scale);
};

