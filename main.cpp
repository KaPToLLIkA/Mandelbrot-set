#include <thread>
#include <mutex>
#include <future>

#include "imgui.h"
#include "imgui-SFML.h"
#include "SFML/Graphics.hpp"
#include "portable-file-dialogs.h"

#include "MandelSet.h"
#include "typedef.h"
#include "utils_main.h"
#include "windows.h"

#ifdef DEBUG
#include "time_test.h"
#include <iostream>
#endif // DEBUG









namespace params 
{
	std::thread *new_thread;
	
	sf::Vector2u	menu_window_size = { 600, 800 };


	std::mutex lock_menu;
	bool menu_notified            = true;
	bool opened_mandelbrot_window = false;
	bool menu_window_closed       = false;
	bool menu_window_rolled       = false;


	sf::Font font;
}

namespace fractl 
{
	MandelSet *mandelbrot;


}








void MandelbrotView();



int main() 
{
	params::font.loadFromFile("data\\monof55.ttf");
	


	GE background;
	while (!params::menu_window_closed)
	{
		while (!params::menu_notified)
		{
			params::lock_menu.lock();
			params::lock_menu.unlock();
		}
		params::menu_notified = false;
		delete params::new_thread;


		sf::RenderWindow menu_window(sf::VideoMode(params::menu_window_size.x, params::menu_window_size.y), " ", sf::Style::Default);
		menu_window.setVerticalSyncEnabled(true);
		menu_window.setFramerateLimit(60);
		ImGui::SFML::Init(menu_window, false);
		LoadCustomFont();

		sf::Clock deltaClock;
		while (menu_window.isOpen())
		{
			


			
			sf::Event events_handler;

			while (menu_window.pollEvent(events_handler))
			{
				ImGui::SFML::ProcessEvent(events_handler);
				if (events_handler.type == sf::Event::Closed) params::menu_window_closed = true;
				if (events_handler.type == sf::Event::Resized) menu_window.setSize(params::menu_window_size);

			}
			ImGui::SFML::Update(menu_window, deltaClock.restart());
			
			//menu section
			{
				ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[FontT::middle]);
				ImGui::SetNextWindowPos({ 0.0f, 0.0f }, ImGuiCond_::ImGuiCond_FirstUseEver);
				ImGui::Begin("Menu", nullptr, params::menu_window_size, 0.0f,
					ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground |
					ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration |
					ImGuiWindowFlags_::ImGuiWindowFlags_NoMove |
					ImGuiWindowFlags_::ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_::ImGuiWindowFlags_NoSavedSettings);

				
				ImGui::SetCursorPos({ params::menu_window_size.x*0.3f, params::menu_window_size.y*0.4f });
				if(ImGui::Button("Mandelbrot", { params::menu_window_size.x * 0.4f, 30.f }))
				{
					fractl::mandelbrot = new MandelSet(
						{ FRAME_MANDEL_X >> 1, FRAME_MANDEL_Y >> 1 },
						{ FRAME_MANDEL_X, FRAME_MANDEL_Y }, 
						MANDEL_RENDER_THRDS_NUM,
						MANDEL_UPDATE_THRDS_NUM
					);

					params::opened_mandelbrot_window = true;
					params::menu_window_rolled = true;
					params::new_thread = new std::thread(MandelbrotView);
					params::new_thread->detach();

					
					std::async(std::launch::async, &MandelSet::InitRenderer, fractl::mandelbrot);
				}

				ImGui::SetCursorPos({ params::menu_window_size.x*0.3f, params::menu_window_size.y - 60.f });
				if (ImGui::Button("Close", { params::menu_window_size.x * 0.4f, 30.f }))
				{
					params::menu_window_closed = true;
				}

				ImGui::End();
				
				ImGui::PopFont();
			}
			//end menu section
			menu_window.clear();



			ImGui::SFML::Render(menu_window);
			menu_window.display();
			if (params::menu_window_closed || params::menu_window_rolled)
			{
				//saving settings

				menu_window.close();
				
			}


		}
	}

	return 0;
}


void MandelbrotView()
{
	params::lock_menu.lock();


	std::string scale_str    = "scale: " + std::to_string(fractl::mandelbrot->getScale());
	std::string iter_num_str = "iterations: " + std::to_string(fractl::mandelbrot->getNumberOfIterations());
	bool restart_render       = false;
	bool mouse_button_pressed = false;

	sf::Vector2i last_mouse_pos;
	sf::Vector2i cur_mouse_pos;

	sf::RenderWindow window(sf::VideoMode(FRAME_MANDEL_X, FRAME_MANDEL_Y), " ", sf::Style::Default);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);
	
	//SECTION "GRAPHIC ELEMENTS INITIALIZATION"
	GE center("data\\textures\\center.png");
	center.sprite.setOrigin(10.f, 10.f);
	center.sprite.setPosition(
		{ 
			static_cast<float>(FRAME_MANDEL_X >> 1), 
			static_cast<float>(FRAME_MANDEL_Y >> 1)
		}
	);
	//END OF SECTION "GRAPHIC ELEMENTS INITIALIZATION"
	
	
	auto ret_val = std::async(
		std::launch::async,
		MandelSettingsWindow,
		&window,
		fractl::mandelbrot,
		&params::opened_mandelbrot_window,
		std::ref(params::font)
	);

	while (window.isOpen())
	{




		sf::Event events_handler;
		cur_mouse_pos = sf::Mouse::getPosition();

		while (window.pollEvent(events_handler))
		{
			if (events_handler.type == sf::Event::Closed) params::opened_mandelbrot_window = false;
			if (events_handler.type == sf::Event::Resized) window.setSize({FRAME_MANDEL_X, FRAME_MANDEL_Y});

			if (events_handler.type == sf::Event::MouseWheelMoved)
			{

				if (events_handler.mouseWheel.delta == -1)
				{
					fractl::mandelbrot->ZoomIncrease();
					scale_str = "scale: " + std::to_string(fractl::mandelbrot->getScale());
					restart_render = true;

				}
				else if (events_handler.mouseWheel.delta == 1)
				{
					fractl::mandelbrot->ZoomDecrease();
					scale_str = "scale: " + std::to_string(fractl::mandelbrot->getScale());
					restart_render = true;
				}

			}
			if (events_handler.type == sf::Event::MouseButtonPressed)
			{
				mouse_button_pressed = true;
				last_mouse_pos = cur_mouse_pos;
				std::async(std::launch::async, &MandelSet::StopRenderer, fractl::mandelbrot);

			}
			if (events_handler.type == sf::Event::MouseButtonReleased)
			{
				mouse_button_pressed = false;
				fractl::mandelbrot->current_frame.sprite.setPosition(
					{FRAME_MANDEL_X >> 1, FRAME_MANDEL_Y >> 1});
				std::async(std::launch::async, &MandelSet::StartRenderer, fractl::mandelbrot);
				
			}


		}
		if (restart_render)
		{
			std::async(std::launch::async, &MandelSet::RestartRenderer, fractl::mandelbrot);
			restart_render = false;
		}
		if (mouse_button_pressed)
		{
			fractl::mandelbrot->Update_delta_Mx_My(last_mouse_pos, cur_mouse_pos);
		}
		else
		{

			fractl::mandelbrot->current_frame.locker.lock();
			fractl::mandelbrot->current_frame.texture.loadFromImage(
				fractl::mandelbrot->current_frame.image
			);
			fractl::mandelbrot->current_frame.locker.unlock();

			fractl::mandelbrot->current_frame.sprite.setTexture(
				fractl::mandelbrot->current_frame.texture
			);
		}



		window.clear();
		window.draw(fractl::mandelbrot->current_frame.sprite);
		window.draw(showBoldText20(&scale_str, { static_cast<float>(FRAME_MANDEL_X >> 1), 5.f }, params::font));
		window.draw(showBoldText20(&iter_num_str, {5.f, 5.f}, params::font));
		window.draw(center.sprite);

		window.display();

		
		if (!params::opened_mandelbrot_window)
		{
			params::menu_window_rolled = false;
			params::menu_notified = true;

			fractl::mandelbrot->DestroyRenderer();
			delete fractl::mandelbrot;

			window.close();
		}

		last_mouse_pos = cur_mouse_pos;
	}
	params::lock_menu.unlock();
	ret_val.wait();
}


