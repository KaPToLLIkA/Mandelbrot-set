#include <iostream>
#include <thread>
#include <mutex>
#include <sstream>
#include <fstream>

#include "imgui.h"
#include "imgui-SFML.h"
#include "mandelbrot_set.h"

#define IM_X 600
#define IM_Y 600
#define THREADS_COUNT 8
#define NUMBER_OF_COLOR_POINTS 4



#include "time_test.h"





namespace params {

	sf::Vector2u	main_window_size = { IM_X + 400, IM_Y },
					img_size = { IM_X, IM_Y },
					render_size = { 150, 300 };

	sf::Vector2f	img_pos = { float(img_size.x >> 1), float(img_size.y >> 1) };
													//x     y
	std::vector <sf::Vector2u> render_positions = { { 0,	0 },
													{ 150,	0 },
													{ 300,	0 },
													{ 450,	0 },
													{ 0,	300 },
													{ 150,	300 },
													{ 300,	300 }, 
													{ 450,	300 } };
	int **koeffs = new int* [IM_Y];
	
	double		scale = 1.0f,
				radius = 8.0f,
				delta_mx = 0.0f,
				delta_my = 0.0f,
				mx = Mandelbrot::countMx_My(0, main_window_size.x - 1, scale, delta_mx),
				my = Mandelbrot::countMx_My(0, main_window_size.y - 1, scale, delta_my);
	unsigned	number_of_iterations = NUMBER_OF_COLOR_POINTS * 12,
				number_of_render_threads = THREADS_COUNT,
				number_of_saved_img = 0;

	std::vector <std::thread*> threads;
	
}












Mandelbrot::MandelbrotSet set(params::img_size, params::img_pos, params::number_of_render_threads, NUMBER_OF_COLOR_POINTS);
sf::RenderWindow main_window(sf::VideoMode(params::main_window_size.x, params::main_window_size.y), " ", sf::Style::Default);

bool	should_generate_img = true;


void mandelbrotSetRenderThread(unsigned);
void updateSprite();
void saveFrame();
void createRenderThreads();


int main() {
	assert (!(NUMBER_OF_COLOR_POINTS < 4) && "NUMBER OF COLOR POINTS SHOULD BE GREATER THAN 3");

	for (unsigned i = 0; i < IM_Y; ++i)
		params::koeffs[i] = new int[IM_X];
	main_window.setVerticalSyncEnabled(true);
	

	set.color_point[0] = { (uint8_t)120, (uint8_t)240, (uint8_t)54, (uint8_t)255 };
	set.color_point[1] = { (uint8_t)120, (uint8_t)135, (uint8_t)84, (uint8_t)189 };
	set.color_point[2] = { (uint8_t)63, (uint8_t)240, (uint8_t)144, (uint8_t)215 };
	set.color_point[3] = { (uint8_t)213, (uint8_t)22, (uint8_t)199, (uint8_t)15 };
	

	double s = getCPUTime();
	set.createColorsTable(params::number_of_iterations);
	double e = getCPUTime();
	std::cout << e - s << std::endl;

	createRenderThreads();
	std::this_thread::sleep_for(std::chrono::seconds(2));
	



	sf::Text text;
	sf::Font font;
	font.loadFromFile("12144.ttf");
	text.setFont(font);
	text.setOrigin(0.0f, 0.0f);
	text.setStyle(sf::Text::Style::Bold);
	text.setFillColor(sf::Color::White);
	text.setPosition(5.0f, 5.0f);
	text.setCharacterSize(30);


	std::stringstream converter;
	std::string text_string;
	converter << params::number_of_iterations;
	converter >> text_string;
	converter.clear();
	text.setString(text_string);

	sf::Image center_img;
	center_img.loadFromFile("center.png");
	sf::Texture center_tex;
	center_tex.loadFromImage(center_img);
	sf::Sprite center_sp;
	center_sp.setTexture(center_tex);
	center_sp.setOrigin(20.0f, 20.0f);
	center_sp.scale(0.4f, 0.4f);
	center_sp.setPosition((float)params::main_window_size.x / 2.0f, (float)params::main_window_size.y / 2.0f);





	sf::Vector2i last_mouse_pos;
	bool main_window_closed = false;
	bool update_sprite = true;
	bool mouse_button_pressed = false;
	int i = 0;


	main_window.setFramerateLimit(60);
	ImGui::SFML::Init(main_window, true);
	sf::Clock deltaClock;
	while (main_window.isOpen()) {
		sf::Vector2i current_mouse_pos = sf::Mouse::getPosition();


		sf::Event events_handler;
		while (main_window.pollEvent(events_handler)) {
			ImGui::SFML::ProcessEvent(events_handler);

			if (events_handler.type == sf::Event::Closed) main_window_closed = true;
			if (events_handler.type == sf::Event::Resized) main_window.setSize(params::main_window_size);
			if (events_handler.type == sf::Event::MouseWheelMoved) {
				if (events_handler.mouseWheel.delta == -1)
				{
					params::scale *= 1.2f;
					update_sprite = true;
				}
				else if (events_handler.mouseWheel.delta == 1)
				{
					
					params::scale /= 1.2f;
					update_sprite = true;
				}
				
			}
			if (events_handler.type == sf::Event::MouseButtonPressed) {
				mouse_button_pressed = true;
				last_mouse_pos = current_mouse_pos;
			}
			if (events_handler.type == sf::Event::MouseButtonReleased) {
				mouse_button_pressed = false;
				update_sprite = true;
			}
		}
		ImGui::SFML::Update(main_window, deltaClock.restart());

		

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			params::number_of_iterations += NUMBER_OF_COLOR_POINTS;
			converter << params::number_of_iterations;
			converter >> text_string;
			converter.clear();
			text.setString(text_string);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			params::number_of_iterations -= NUMBER_OF_COLOR_POINTS;
			if (params::number_of_iterations < NUMBER_OF_COLOR_POINTS) 
				params::number_of_iterations = NUMBER_OF_COLOR_POINTS;


			converter << params::number_of_iterations;
			converter >> text_string;
			converter.clear();
			text.setString(text_string);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) saveFrame();
		
		if (mouse_button_pressed) {
			params::delta_mx += Mandelbrot::countDeltaMx_My(last_mouse_pos.x, current_mouse_pos.x, params::scale);
			params::delta_my += Mandelbrot::countDeltaMx_My(last_mouse_pos.y, current_mouse_pos.y, params::scale);
			set.sp.setPosition(set.sp.getPosition().x + (last_mouse_pos.x - current_mouse_pos.x) * -1,
								set.sp.getPosition().y + (last_mouse_pos.y - current_mouse_pos.y) * -1);
		}
		last_mouse_pos = current_mouse_pos;
		if (update_sprite) {
			std::cout << "render" << std::endl;
			sf::Clock clock;
			set.sp.setPosition(params::img_pos);
			params::mx = Mandelbrot::countMx_My(0, params::main_window_size.x - 1, params::scale, params::delta_mx);
			params::my = Mandelbrot::countMx_My(0, params::main_window_size.y - 1, params::scale, params::delta_my);
			updateSprite();
			update_sprite = false;
			std::cout << " seconds_passed " << clock.getElapsedTime().asSeconds() << std::endl << std::endl;
		}




		main_window.clear(sf::Color(63, 63, 63, 255));
		main_window.draw(set.sp);
		main_window.draw(text);
		main_window.draw(center_sp);
		ImGui::SFML::Render(main_window);
		main_window.display();

		if (main_window_closed) {
			//saving settings

			should_generate_img = false;
			main_window.close();
		}
	

	}
	updateSprite();
	for (unsigned i = 0; i < params::number_of_render_threads; i++) 
		set.thread_lockers[i].unlock();
	
	for (size_t i = 0; i < IM_Y; ++i)
		delete[] params::koeffs[i];

	delete[] params::koeffs;

	return 0;
}


















void mandelbrotSetRenderThread(unsigned id) {
	
	while (main_window.isOpen()) {
		set.wait[id].lock();
		set.thread_lockers[id].lock();
		set.wait[id].unlock();
		set.img_lockers[id].lock();
		set.thread_lockers[id].unlock();
		if (should_generate_img) {
			Mandelbrot::createKoeffsPart(params::koeffs,
										params::render_positions[id],
										params::render_size,
										params::scale,
										params::mx,
										params::my,
										params::number_of_iterations,
										params::radius);
		}
		set.img_lockers[id].unlock();
	}
}













void updateSprite()
{
	unsigned i;
	for (i = 0; i < params::number_of_render_threads; i++) set.thread_lockers[i].unlock();
	for (i = 0; i < params::number_of_render_threads; i++) set.wait[i].lock();
	for (i = 0; i < params::number_of_render_threads; i++) set.thread_lockers[i].lock();
	for (i = 0; i < params::number_of_render_threads; i++) set.wait[i].unlock();
	for (i = 0; i < params::number_of_render_threads; i++) set.img_lockers[i].lock();

	double s = getCPUTime();
	set.updatePixels(params::koeffs, IM_X, IM_Y);
	double e = getCPUTime();
	std::cout << e - s << std::endl;
	set.tex.loadFromImage(set.img);
	set.sp.setTexture(set.tex);
	set.sp.setPosition(params::img_pos);

	for (i = 0; i < params::number_of_render_threads; i++) set.img_lockers[i].unlock();
}












void saveFrame()
{
	std::stringstream converter;
	std::string idx_str;
	converter << params::number_of_saved_img;
	converter >> idx_str;
	converter.clear();
	set.img.saveToFile("frames\\frame" + idx_str + ".jpg");
	params::number_of_saved_img += 1;
	std::cout << "frame " << idx_str << " saved" << std::endl;
}












void createRenderThreads()
{

	for (size_t i = 0; i < THREADS_COUNT; ++i)
	{
		std::thread* thread = new std::thread(mandelbrotSetRenderThread, i);
		thread->detach();
		params::threads.push_back(thread);
	}
}
