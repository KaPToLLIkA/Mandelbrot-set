#include <thread>
#include <mutex>
#include <future>

#include "imgui.h"
#include "imgui-SFML.h"
#include "SFML/Graphics.hpp"
#include "portable-file-dialogs.h"

#include "MandelSet.h"
#include "typedef.h"

#ifdef DEBUG
#include "time_test.h"
#include <iostream>
#endif // DEBUG









namespace params 
{
	std::thread *new_thread;
	
	sf::Vector2u	menu_window_size = { 600, 800 };


	std::mutex lock_menu;
	bool notified = true;
	bool opened_mandelbrot_window = false;
	bool menu_window_closed = false;
	bool menu_window_rolled = false;
}

namespace fractl 
{
	MandelSet *mandelbrot;


}








void MandelbrotView();



int main() 
{
	Frame bg;
	while (!params::menu_window_closed)
	{
		while (!params::notified)
		{
			params::lock_menu.lock();
			params::lock_menu.unlock();
		}
		params::notified = false;
		delete params::new_thread;


		sf::RenderWindow menu_window(sf::VideoMode(params::menu_window_size.x, params::menu_window_size.y), " ", sf::Style::Default);
		menu_window.setVerticalSyncEnabled(true);
		menu_window.setFramerateLimit(60);
		ImGui::SFML::Init(menu_window, true);


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
	bool restart_render = false;
	params::lock_menu.lock();
	sf::RenderWindow window(sf::VideoMode(FRAME_MANDEL_X, FRAME_MANDEL_Y), " ", sf::Style::Default);

	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);
	
	while (window.isOpen())
	{




		sf::Event events_handler;

		while (window.pollEvent(events_handler))
		{
			if (events_handler.type == sf::Event::Closed) params::opened_mandelbrot_window = false;
			if (events_handler.type == sf::Event::Resized) window.setSize(params::menu_window_size);

			if (events_handler.type == sf::Event::MouseWheelMoved)
			{

				if (events_handler.mouseWheel.delta == -1)
				{
					fractl::mandelbrot->ZoomIncrease();
					restart_render = true;

				}
				else if (events_handler.mouseWheel.delta == 1)
				{
					fractl::mandelbrot->ZoomDecrease();
					restart_render = true;
				}

			}
			if (events_handler.type == sf::Event::MouseButtonPressed)
			{




			}
			if (events_handler.type == sf::Event::MouseButtonReleased)
			{


			}


		}
		if (restart_render)
		{
			std::async(std::launch::async, &MandelSet::RestartRenderer, fractl::mandelbrot);
			restart_render = false;
		}

		

		fractl::mandelbrot->current_frame.locker.lock();
		fractl::mandelbrot->current_frame.texture.loadFromImage(
			fractl::mandelbrot->current_frame.image
		);
		fractl::mandelbrot->current_frame.locker.unlock();
		
		fractl::mandelbrot->current_frame.sprite.setTexture(
			fractl::mandelbrot->current_frame.texture
		);

		window.clear();
		window.draw(fractl::mandelbrot->current_frame.sprite);


		window.display();
		if (!params::opened_mandelbrot_window)
		{
			params::menu_window_rolled = false;
			params::notified = true;

			fractl::mandelbrot->DestroyRenderer();
			delete fractl::mandelbrot;

			window.close();
		}


	}
	params::lock_menu.unlock();
}