#include <iostream>
#include <thread>
#include <mutex>
#include <sstream>
#include <fstream>
#include <future>
#include <ctime>
#include <array>
#include <codecvt>
#include <filesystem>

#include "imgui.h"
#include "imgui-SFML.h"
#include "SFML/Graphics.hpp"
#include "portable-file-dialogs.h"

#include "MandelSet.h"
#define IM_X 600
#define IM_Y 800
#define THREADS_COUNT 4









namespace params {

	
	sf::Vector2u	main_window_size = { IM_X, IM_Y },
					settings_window_size = {400, IM_Y};


	bool settings_window_closed = true;
	bool main_window_closed = false;
	bool color_change = false;
	bool frame_saved = false;
	bool windows_magnet = false;
	
}











sf::RenderWindow main_window(sf::VideoMode(params::main_window_size.x, params::main_window_size.y), " ", sf::Style::Default);
sf::RenderWindow settings_window;


MandelSet set({ 0,0 }, {IM_X, IM_Y});

int main() 
{
	
	main_window.setVerticalSyncEnabled(true);
	main_window.setFramerateLimit(60);
	//set.render_engine = new RenderThreads<MandelSet>(4, set, MandelSet::);


	
	


	
	sf::Clock deltaClock;
	sf::Clock deltaRender;
	deltaRender.restart();
	while (main_window.isOpen()) 
	{
		sf::Vector2i current_mouse_pos = {0,0};
		
		
		{
			sf::Event events_handler;
		
			current_mouse_pos = sf::Mouse::getPosition();







			while (main_window.pollEvent(events_handler))
			{


				if (events_handler.type == sf::Event::Closed) params::main_window_closed = true;
				if (events_handler.type == sf::Event::Resized) main_window.setSize(params::main_window_size);


				if (events_handler.type == sf::Event::MouseWheelMoved)
				{

					if (events_handler.mouseWheel.delta == -1)
					{
						
						
					}
					else if (events_handler.mouseWheel.delta == 1)
					{

						
						


					}

				}
				if (events_handler.type == sf::Event::MouseButtonPressed)
				{

					


				}
				if (events_handler.type == sf::Event::MouseButtonReleased)
				{

					
				}

			}
		}

		/*main_window.clear({	sf::Uint8((c1.r + c2.r + c3.r + c4.r) / 4),
							sf::Uint8((c1.g + c2.g + c3.g + c4.g) / 4),
							sf::Uint8((c1.b + c2.b + c3.b + c4.b) / 4),
							sf::Uint8((c1.a + c2.a + c3.a + c4.a) / 4) });*/
		main_window.clear();
		
		
		main_window.display();
		if (params::main_window_closed) 
		{
			//saving settings
			
			main_window.close();
		}
	

	}
	
	
	
	

	return 0;
}
