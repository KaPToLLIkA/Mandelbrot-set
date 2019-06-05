#include "MandelSet.h"


////////////////////////////////////////////////////////////////////////////
void MandelSet::isInMandelbrotSet(
	std::complex <double>  z,
	std::complex <double> *c,
	double				  &radius,
	unsigned		       max_iter,
	unsigned			  *current_iter)
{
	if (abs(z) < radius && *current_iter < max_iter) {
		*current_iter += 1;
		isInMandelbrotSet(((z*z) + *c), c, radius, max_iter, current_iter);
	}
}

////////////////////////////////////////////////////////////////////////////
std::complex <double> MandelSet::findComplex(
	unsigned x,
	unsigned y,
	double   &scale,
	double   &mx,
	double   &my)
{
	return std::complex <double>((double)x * scale * 1.0f / 200.0f + mx,
		(double)y * scale * 1.0f / 200.0f + my);

}

////////////////////////////////////////////////////////////////////////////
double MandelSet::countMx_My(	
	unsigned left,
	unsigned right,
	double   &scale,
	double   &delta)
{
	return ((double)(left)* scale * 1.0f / 200.0f - (double)(right)* scale * 1.0f / 200.0f) / 2.0f + delta;
}

////////////////////////////////////////////////////////////////////////////
double MandelSet::countDeltaMx_My(	
	int    last,
	int    current,
	double &scale)
{
	return (double)(last)* scale * 1.0f / 200.0f - (double)(current)* scale * 1.0f / 200.0f;
}

////////////////////////////////////////////////////////////////////////////
void CreateImgPartMandelbrot(
	MandelSet	   *_this, 
	RenderThreads  *thr,
	size_t			idx)
{
	thr->exit_locker[idx]->lock();
#ifdef DEBUG
	thr->safePrint(
		std::string(
			"[INF_MANDEL] Thread init idx = " + std::to_string(idx) + "\n"
		)
	);
#endif // DEBUG

	
	

	
	while (!thr->exit)
	{
		{
			std::unique_lock <std::mutex> locker(thr->wait_restart);

			while (!thr->notified[idx]) thr->alert.wait(locker);
				thr->notified[idx] = false;
		}
		thr->work_locker_target_thread[idx]->lock();
		thr->render_status[idx] = false;
		unsigned x = 0;
		unsigned y = 0;
		unsigned iteration = 0;
#ifdef DEBUG
		thr->safePrint(
			std::string(
				"[INF_MANDEL] Thread idx = " + std::to_string(idx) + " in work section...\n"
			)
		);
#endif // DEBUG
		while (x < _this->dscr.img_size.x && !thr->stop_render) 
		{
			/*thr->pauser_target_thread[idx]->lock();
			thr->pauser_target_thread[idx]->unlock();*/

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

			if (x >= _this->dscr.img_size.x) break;

			_this->dscr.changing.lock();

			//calculating pixel
			MandelSet::isInMandelbrotSet(std::complex <double>(0.0f, 0.0f),
				&MandelSet::findComplex(x, y, 
					_this->dscr.scale, 
					_this->dscr.mx, 
					_this->dscr.my), 
				_this->dscr.radius, 
				_this->dscr.number_of_iterations, 
				&iteration);
			
			_this->dscr.changing.unlock();

			_this->lock_matrix.lock();
			_this->iter_matrix[x][y] = --iteration;
			_this->lock_matrix.unlock();

			iteration = 0;

		}


		thr->render_status[idx] = true;
		thr->work_locker_target_thread[idx]->unlock();
#ifdef DEBUG
		thr->safePrint(
			std::string(
				"[INF_MANDEL] Thread idx = " + std::to_string(idx) + " end work... \n"
			)
		);
#endif // DEBUG
	}
#ifdef DEBUG
	thr->safePrint(
		std::string(
			"[INF_MANDEL] Thread idx = " + std::to_string(idx) + " shutting down...\n"
		)
	);
#endif // DEBUG
	thr->exit_locker[idx]->unlock();
}

////////////////////////////////////////////////////////////////////////////
void UpdateColorMandelbrot(
	MandelSet	   *_this,
	RenderThreads  *thr,
	size_t			idx)
{
	thr->exit_locker[idx]->lock();
#ifdef DEBUG
	thr->safePrint(
		std::string(
			"[INF_MANDEL_COLORS] Thread init idx = " + std::to_string(idx) + "\n"
		)
	);
#endif // DEBUG
	
	
	
	while (!thr->exit)
	{
		{
			std::unique_lock <std::mutex> locker(thr->wait_restart);

			while (!thr->notified[idx]) thr->alert.wait(locker);
			thr->notified[idx] = false;
		}
		unsigned x = 0;
		unsigned y = 0;
		thr->work_locker_target_thread[idx]->lock();
		thr->render_status[idx] = false;
#ifdef DEBUG
		thr->safePrint(
			std::string(
				"[INF_MANDEL_COLORS] Thread idx = " + std::to_string(idx) + " in work section...\n"
			)
		);
#endif // DEBUG
		while (x < _this->dscr.img_size.x && !thr->stop_render)
		{
			/*thr->pauser_target_thread[idx]->lock();
			thr->pauser_target_thread[idx]->unlock();*/

			_this->dscr.lock_next_xy_colors.lock();
			y = _this->dscr.next_y_colors;
			x = _this->dscr.next_x_colors;

			++_this->dscr.next_y_colors;

			if (_this->dscr.next_y_colors == _this->dscr.img_size.y)
			{

				++_this->dscr.next_x_colors;
				_this->dscr.next_y_colors = 0;

			}
			_this->dscr.lock_next_xy_colors.unlock();

			if (x >= _this->dscr.img_size.x) break;

			while ((x >= _this->dscr.next_x
				|| _this->iter_matrix[x][y] == -1)
				&& !thr->stop_render)
				std::this_thread::sleep_for(std::chrono::microseconds(250));
			if (thr->stop_render) break;

			_this->lock_matrix.lock();
			_this->current_frame.locker.lock();
			_this->lock_color_table.lock();
			_this->current_frame.image.setPixel(
				x, y,
				sf::Color(_this->color_table[_this->iter_matrix[x][y]]));

			_this->lock_color_table.unlock();
			_this->current_frame.locker.unlock();
			_this->lock_matrix.unlock();

		}
		thr->render_status[idx] = true;
		thr->work_locker_target_thread[idx]->unlock();
#ifdef DEBUG
		thr->safePrint(
			std::string(
				"[INF_MANDEL_COLORS] Thread idx = " + std::to_string(idx) + " end work... \n"
			)
		);
#endif // DEBUG
	}
#ifdef DEBUG
	thr->safePrint(
		std::string(
			"[INF_MANDEL_COLORS] Thread idx = " + std::to_string(idx) + " shutting down...\n"
		)
	);
#endif // DEBUG
	thr->exit_locker[idx]->unlock();
}

////////////////////////////////////////////////////////////////////////////
void MandelSet::ResetRenderParams()
{
	dscr.changing.lock();
	dscr.next_x = 0;
	dscr.next_y = 0;
	dscr.next_x_colors = 0;
	dscr.next_y_colors = 0;

	dscr.mx = MandelSet::countMx_My(0, dscr.img_size.x - 1, dscr.scale, dscr.delta_mx);
	dscr.my = MandelSet::countMx_My(0, dscr.img_size.y - 1, dscr.scale, dscr.delta_my);
	dscr.changing.unlock();
}

////////////////////////////////////////////////////////////////////////////
void MandelSet::ClearIterMatrix()
{
	for (unsigned x = 0; x < dscr.img_size.x; ++x)
		for (unsigned y = 0; y < dscr.img_size.y; ++y)
			iter_matrix[x][y] = -1;
}

////////////////////////////////////////////////////////////////////////////
void MandelSet::allocMatrix(sf::Vector2u img_size)
{
	iter_matrix = new unsigned*[img_size.x];
	for (size_t i = 0; i < img_size.x; ++i)
	{
		iter_matrix[i] = new unsigned[img_size.y];
	}
}

////////////////////////////////////////////////////////////////////////////
void MandelSet::deleteMatrix()
{
	for (size_t i = 0; i < dscr.img_size.x; ++i)
		delete[] iter_matrix[i];

	delete[] iter_matrix;
}

////////////////////////////////////////////////////////////////////////////
void MandelSet::PlusCalcDeltaColors(
	size_t min, 
	size_t max,
	float delta_iter,
	color::ColorFloat start, 
	color::ColorFloat end)
{
	color::ColorFloat delta = end - start;
	delta.setR(delta.R() / delta_iter);
	delta.setG(delta.G() / delta_iter);
	delta.setB(delta.B() / delta_iter);
	delta.setA(delta.A() / delta_iter);

	color_table[min] = start;
	++min;
	for (min; min < max; ++min) {
		color_table[min] = color_table[min - 1] + delta;
	}
}

////////////////////////////////////////////////////////////////////////////
void MandelSet::MinusCalcDeltaColors(
	size_t min, 
	size_t max,
	float delta_iter,
	color::ColorFloat start, 
	color::ColorFloat end)
{
	color::ColorFloat delta = start - end;
	delta.setR(delta.R() / delta_iter);
	delta.setG(delta.G() / delta_iter);
	delta.setB(delta.B() / delta_iter);
	delta.setA(delta.A() / delta_iter);

	color_table[min] = start;
	++min;
	for (min; min < max; ++min) {
		color_table[min] = color_table[min - 1] - delta;
	}
}

////////////////////////////////////////////////////////////////////////////
MandelSet::MandelSet(
	sf::Vector2f img_pos, 
	sf::Vector2u img_size, 
	size_t		 number_of_threads,
	size_t       number_of_update_threads)
{
	dscr.img_size = img_size;
	dscr.img_pos = img_pos;
	dscr.mx = MandelSet::countMx_My(0, img_size.x - 1, dscr.scale, dscr.delta_mx);
	dscr.my = MandelSet::countMx_My(0, img_size.y - 1, dscr.scale, dscr.delta_my);
	
	render_engine = new RenderThreads(
		number_of_threads, this, CreateImgPartMandelbrot
	);

	update_colors_engine = new RenderThreads(
		number_of_update_threads, this, UpdateColorMandelbrot
	);



	current_frame.image.create(img_size.x, img_size.y);
	current_frame.sprite.setOrigin(
		static_cast<float>(img_size.x >> 1), 
		static_cast<float>(img_size.y >> 1)
	);
	current_frame.sprite.setPosition(img_pos);

	


	allocMatrix(img_size);
	ClearIterMatrix();
}

////////////////////////////////////////////////////////////////////////////
MandelSet::~MandelSet()
{
	deleteMatrix();
}

////////////////////////////////////////////////////////////////////////////
double MandelSet::getScale()
{
	return dscr.scale;
}

////////////////////////////////////////////////////////////////////////////
unsigned MandelSet::getNumberOfIterations()
{
	return dscr.number_of_iterations;
}

////////////////////////////////////////////////////////////////////////////
void MandelSet::InitRenderer()
{
	createGradientColorsTable();
	render_engine->StartAllThreads();
	update_colors_engine->StartAllThreads();
}

////////////////////////////////////////////////////////////////////////////
void MandelSet::DestroyRenderer()
{
	lock_destroy.lock();
	render_engine->DestroyThreads();
	update_colors_engine->DestroyThreads();
	lock_destroy.unlock();
}

////////////////////////////////////////////////////////////////////////////
void MandelSet::RestartRenderer()
{
	lock_destroy.lock();
	render_engine->StopAllThreads();
	update_colors_engine->StopAllThreads();


	ClearIterMatrix();
	ResetRenderParams();

	render_engine->StartAllThreads();
	update_colors_engine->StartAllThreads();
	lock_destroy.unlock();
}

////////////////////////////////////////////////////////////////////////////
void MandelSet::StopRenderer()
{
	lock_destroy.lock();
	render_engine->StopAllThreads();
	update_colors_engine->StopAllThreads();

	lock_destroy.unlock();
}

void MandelSet::StartRenderer()
{
	lock_destroy.lock();
	ClearIterMatrix();
	ResetRenderParams();



	render_engine->StartAllThreads();
	update_colors_engine->StartAllThreads();
	lock_destroy.unlock();
}

bool MandelSet::CheckRenderStatus()
{
	return render_engine->CheckStatusFromAll() && update_colors_engine->CheckStatusFromAll();
}

////////////////////////////////////////////////////////////////////////////
void MandelSet::ZoomIncrease()
{
	dscr.changing.lock();
	dscr.scale *= MANDEL_ZOOM_KOEFF;
	dscr.changing.unlock();
}

////////////////////////////////////////////////////////////////////////////
void MandelSet::ZoomDecrease()
{
	dscr.changing.lock();
	dscr.scale /= MANDEL_ZOOM_KOEFF;
	dscr.changing.unlock();
}

////////////////////////////////////////////////////////////////////////////
void MandelSet::Update_delta_Mx_My(
	sf::Vector2i &last_mouse_pos, 
	sf::Vector2i &cur_mouse_pos)
{
	dscr.changing.lock();
	dscr.delta_mx += countDeltaMx_My(
		last_mouse_pos.x, 
		cur_mouse_pos.x, 
		dscr.scale);

	dscr.delta_my += countDeltaMx_My(
		last_mouse_pos.y,
		cur_mouse_pos.y,
		dscr.scale);
	dscr.changing.unlock();


	current_frame.sprite.setPosition(
		current_frame.sprite.getPosition().x + cur_mouse_pos.x - last_mouse_pos.x,
		current_frame.sprite.getPosition().y + cur_mouse_pos.y - last_mouse_pos.y);
}

////////////////////////////////////////////////////////////////////////////
#define BLACK   {1.f, 1.f, 1.f, 1.f} //1,10
#define WHITE   {0.f, 0.f, 0.f, 1.f} //2
#define RED     {1.f, 0.f, 0.f, 1.f} //3,9
#define MAGENTA {1.f, 0.f, 1.f, 1.f} //4
#define BLUE    {0.f, 0.f, 1.f, 1.f} //5
#define CYAN    {0.f, 1.f, 1.f, 1.f} //6
#define GREEN   {0.f, 1.f, 0.f, 1.f} //7
#define YELLOW  {1.f, 1.f, 0.f, 1.f} //8

void MandelSet::createGradientColorsTable()
{
	lock_color_table.lock();
	color_table.clear();
	color_table.resize(dscr.number_of_iterations);

	float delta_iter = static_cast<float>(color_table.size()) / 9.0f;

	size_t max_iter1 = static_cast<size_t>(round(delta_iter * 1.0f));
	size_t max_iter2 = static_cast<size_t>(round(delta_iter * 2.0f));
	size_t max_iter3 = static_cast<size_t>(round(delta_iter * 3.0f));
	size_t max_iter4 = static_cast<size_t>(round(delta_iter * 4.0f));
	size_t max_iter5 = static_cast<size_t>(round(delta_iter * 5.0f));
	size_t max_iter6 = static_cast<size_t>(round(delta_iter * 6.0f));
	size_t max_iter7 = static_cast<size_t>(round(delta_iter * 7.0f));
	size_t max_iter8 = static_cast<size_t>(round(delta_iter * 8.0f));

	//                    min        max                 delta       start    end
	MinusCalcDeltaColors (0,         max_iter1,          delta_iter, BLACK,   WHITE  );
	PlusCalcDeltaColors  (max_iter1, max_iter2,          delta_iter, WHITE,   RED    );
	PlusCalcDeltaColors  (max_iter2, max_iter3,          delta_iter, RED,     MAGENTA);
	MinusCalcDeltaColors (max_iter3, max_iter4,          delta_iter, MAGENTA, BLUE   );
	PlusCalcDeltaColors  (max_iter4, max_iter5,          delta_iter, BLUE,    CYAN   );
	MinusCalcDeltaColors (max_iter5, max_iter6,          delta_iter, CYAN,    GREEN  );
	PlusCalcDeltaColors  (max_iter6, max_iter7,          delta_iter, GREEN,   YELLOW );
	MinusCalcDeltaColors (max_iter7, max_iter8,          delta_iter, YELLOW,  RED    );
	PlusCalcDeltaColors  (max_iter8, color_table.size(), delta_iter, RED,     BLACK  );

	lock_color_table.unlock();
}

////////////////////////////////////////////////////////////////////////////
void MandelSet::saveFrame(std::string path)
{
	std::time_t cur_time = std::time(nullptr);
	std::string time_str = std::ctime(&cur_time);

	for (size_t i = 0; i < time_str.size(); ++i)
		if (time_str[i] == ':') time_str[i] = '.';

	time_str.erase(--time_str.end());
	current_frame.locker.lock();
	current_frame.image.saveToFile(path + time_str + ".jpg");
	current_frame.locker.unlock();
}

////////////////////////////////////////////////////////////////////////////
#define MANDEL_CR_FRAMES_WINDOW_X 600
#define MANDEL_CR_FRAMES_WINDOW_Y 700
void MandelSet::createFrames(
	Range<double> scale_r, 
	Range<unsigned> iter_r, 
	sf::Vector2u frame_sz, 
	unsigned number_of_frames,
	sf::Font &font_to_text,
	std::mutex *frame_locker)
{


	calculating_funct_started = true;
	MandelSet tmp_obj(
		{ MANDEL_CR_FRAMES_WINDOW_X >> 1, MANDEL_CR_FRAMES_WINDOW_Y >> 1},
		frame_sz, 
		MANDEL_RENDER_THRDS_NUM,
		MANDEL_UPDATE_THRDS_NUM);

	tmp_obj.createGradientColorsTable();

	this->dscr.changing.lock();
	tmp_obj.dscr.delta_mx = this->dscr.delta_mx;
	tmp_obj.dscr.delta_my = this->dscr.delta_my;
	tmp_obj.dscr.scale = scale_r.min;
	tmp_obj.dscr.number_of_iterations = iter_r.min;
	this->dscr.changing.unlock();
	



	double delta_scale = (scale_r.max - scale_r.min) / static_cast<double>(number_of_frames);
	unsigned delta_iter= (iter_r.max - iter_r.min) / (number_of_frames);

	{
		std::string scale_str = "scale: " + std::to_string(tmp_obj.getScale());
		std::string iter_num_str = "iterations: " + std::to_string(tmp_obj.getNumberOfIterations());
		bool window_opened = true;
		
		sf::RenderWindow window(
			sf::VideoMode(MANDEL_CR_FRAMES_WINDOW_X, MANDEL_CR_FRAMES_WINDOW_Y), "Rendering frames...", sf::Style::Default);
		window.setVerticalSyncEnabled(true);
		window.setFramerateLimit(60);
		ImGui::SFML::Init(window, true);
		

		{
			float k = static_cast<float>(MANDEL_CR_FRAMES_WINDOW_X) / static_cast<float>(frame_sz.x);
			if (k * static_cast<float>(frame_sz.y) > static_cast<float>(MANDEL_CR_FRAMES_WINDOW_Y))
				k = static_cast<float>(MANDEL_CR_FRAMES_WINDOW_Y) / static_cast<float>(frame_sz.y);

			tmp_obj.current_frame.sprite.scale(k, k);
		}

		unsigned frames_counter = 0;
		float percents = 0.f;

		tmp_obj.StartRenderer();

		sf::Clock deltaClock;
		while (window.isOpen())
		{

			frame_locker->lock();



			sf::Event events_handler;

			while (window.pollEvent(events_handler))
			{
				ImGui::SFML::ProcessEvent(events_handler);
				if (events_handler.type == sf::Event::Closed) window_opened = false;
				if (events_handler.type == sf::Event::Resized) window.setSize({MANDEL_CR_FRAMES_WINDOW_X, MANDEL_CR_FRAMES_WINDOW_Y});

			}
			ImGui::SFML::Update(window, deltaClock.restart());
			{
				ImGui::SetNextWindowPos({ 0.0f, MANDEL_CR_FRAMES_WINDOW_Y - 100.f }, ImGuiCond_::ImGuiCond_FirstUseEver);
				ImGui::Begin("RenderStatusBar", nullptr,
					{ MANDEL_CR_FRAMES_WINDOW_X, 100.f }, 0.0f,
					ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration |
					ImGuiWindowFlags_::ImGuiWindowFlags_NoMove |
					ImGuiWindowFlags_::ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_::ImGuiWindowFlags_NoSavedSettings);


				ImGui::ProgressBar(percents, 
					{ 
						MANDEL_CR_FRAMES_WINDOW_X - ImGui::GetStyle().ItemSpacing.x * 2.f, 
						100.f - ImGui::GetStyle().ItemSpacing.y * 2.f 
					});

				ImGui::End();
			}

			window.clear();
			window.draw(tmp_obj.current_frame.sprite);
			window.draw(showBoldText20(&scale_str, { static_cast<float>(FRAME_MANDEL_X >> 1), 5.f }, font_to_text));
			window.draw(showBoldText20(&iter_num_str, { 5.f, 5.f }, font_to_text));


			ImGui::SFML::Render(window);
			window.display();


			if (tmp_obj.CheckRenderStatus())
			{
				++frames_counter;
				percents = static_cast<float>(frames_counter) / static_cast<float>(number_of_frames);
				tmp_obj.saveFrame(".\\data\\frames\\video\\");


				tmp_obj.current_frame.locker.lock();
				tmp_obj.current_frame.texture.loadFromImage(
					tmp_obj.current_frame.image
				);
				tmp_obj.current_frame.locker.unlock();

				tmp_obj.current_frame.sprite.setTexture(
					tmp_obj.current_frame.texture
				);

				this->dscr.changing.lock();
				tmp_obj.dscr.scale += delta_scale;
				tmp_obj.dscr.number_of_iterations += delta_iter;
				this->dscr.changing.unlock();

				tmp_obj.createGradientColorsTable();

				scale_str = "scale: " + std::to_string(tmp_obj.getScale());
				iter_num_str = "iterations: " + std::to_string(tmp_obj.getNumberOfIterations());

				if (frames_counter == number_of_frames) window_opened = false;
				else tmp_obj.StartRenderer();
			}


			if (!window_opened || !calculating_funct_started)
			{
				tmp_obj.DestroyRenderer();
				window.close();
			}

			frame_locker->unlock();
		}

	}
	calculating_funct_started = false;
}


