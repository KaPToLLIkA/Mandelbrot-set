#include <iostream>
#include <thread>
#include <mutex>
#include <sstream>
#include <fstream>

#include "mandelbrot_set.h"

namespace params {

	sf::Vector2u	main_window_size = { 600, 600 },
					img_size = { 600, 600 },
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
	
	
	double		scale = 1.0f,
				radius = 8.0f,
				delta_mx = 0.0f,
				delta_my = 0.0f,
				mx = Mandelbrot::countMx_My(0, main_window_size.x - 1, scale, delta_mx),
				my = Mandelbrot::countMx_My(0, main_window_size.y - 1, scale, delta_my);
	unsigned	number_of_iterations = 64,
				number_of_render_threads = 8,
				number_of_saved_img = 0;

	
	sf::Color color;

}
Mandelbrot::MandelbrotSet set(params::img_size, params::img_pos, params::number_of_render_threads);
sf::RenderWindow main_window(sf::VideoMode(params::main_window_size.x, params::main_window_size.y), " ", sf::Style::Default);

bool	should_generate_img = true;


void mandelbrotSetRenderThread(unsigned);
void updateSprite();
void loadColor();
void saveFrame();

int main() {
	loadColor();
	main_window.setVerticalSyncEnabled(true);
	
	std::thread thread_0(mandelbrotSetRenderThread, 0);
	std::thread thread_1(mandelbrotSetRenderThread, 1);
	std::thread thread_2(mandelbrotSetRenderThread, 2);
	std::thread thread_3(mandelbrotSetRenderThread, 3);
	std::thread thread_4(mandelbrotSetRenderThread, 4);
	std::thread thread_5(mandelbrotSetRenderThread, 5);
	std::thread thread_6(mandelbrotSetRenderThread, 6);
	std::thread thread_7(mandelbrotSetRenderThread, 7);



	thread_0.detach();
	thread_1.detach();
	thread_2.detach();
	thread_3.detach();
	thread_4.detach();
	thread_5.detach();
	thread_6.detach();
	thread_7.detach();

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
	while (main_window.isOpen()) {
		sf::Vector2i current_mouse_pos = sf::Mouse::getPosition();

		main_window.clear(sf::Color(63,63,63,255));

		main_window.draw(set.sp);
		sf::Event events_handler;
		while (main_window.pollEvent(events_handler)) {
			if (events_handler.type == sf::Event::Closed) main_window_closed = true;
			if (events_handler.type == sf::Event::Resized) main_window.setSize(params::main_window_size);
			if (events_handler.type == sf::Event::MouseWheelMoved) {
				if (events_handler.mouseWheel.delta == -1)
				{
					params::scale *= 1.2f;
					
					/*main_window.clear(sf::Color::White);
					set.sp.scale(0.8f, 0.8f);
					main_window.draw(set.sp);
					set.sp.scale(1.0f / 0.8f, 1.0f / 0.8f);*/
					update_sprite = true;
				}
				else if (events_handler.mouseWheel.delta == 1)
				{
					
					params::scale /= 1.2f;
					
					/*main_window.clear(sf::Color::White);
					set.sp.scale(1.2f, 1.2f);
					main_window.draw(set.sp);
					set.sp.scale(1.0f / 1.2f, 1.0f / 1.2f);*/
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
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			params::number_of_iterations += 1;
			converter << params::number_of_iterations;
			converter >> text_string;
			converter.clear();
			text.setString(text_string);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			params::number_of_iterations -= 1;
			converter << params::number_of_iterations;
			converter >> text_string;
			converter.clear();
			text.setString(text_string);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) saveFrame();
		

		main_window.draw(text);
		main_window.draw(center_sp);
		main_window.display();

		
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


		if (main_window_closed) {
			//saving settings

			should_generate_img = false;
			main_window.close();
		}
	

	}
	updateSprite();
	for (unsigned i = 0; i < params::number_of_render_threads; i++) set.thread_lockers[i].unlock();
	

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
			Mandelbrot::createImgPart(set.img,
										params::render_positions[id],
										params::render_size,
										params::scale,
										params::mx,
										params::my,
										params::number_of_iterations,
										params::radius,
										params::color);
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

	set.tex.loadFromImage(set.img);
	set.sp.setTexture(set.tex);
	set.sp.setPosition(params::img_pos);

	for (i = 0; i < params::number_of_render_threads; i++) set.img_lockers[i].unlock();
}

void loadColor()
{
	std::stringstream converter;
	std::ifstream input;
	std::string buffer_str;
	unsigned buffer_i;
	input.open("color_koeffs.txt");
	
	//red
	std::getline(input, buffer_str, ' ');
	converter << buffer_str;
	converter >> buffer_i;
	converter.clear();
	std::getline(input, buffer_str, '\n');
	params::color.r = buffer_i;
	//green
	std::getline(input, buffer_str, ' ');
	converter << buffer_str;
	converter >> buffer_i;
	converter.clear();
	std::getline(input, buffer_str, '\n');
	params::color.g = buffer_i;
	//blue
	std::getline(input, buffer_str, ' ');
	converter << buffer_str;
	converter >> buffer_i;
	converter.clear();
	std::getline(input, buffer_str, '\n');
	params::color.b = buffer_i;
	//alpha
	std::getline(input, buffer_str, ' ');
	converter << buffer_str;
	converter >> buffer_i;
	converter.clear();
	params::color.a = buffer_i;


	input.close();
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
