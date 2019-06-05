#pragma once

#include <future>
#include "imgui.h"
#include "imgui-SFML.h"
#include "SFML/Graphics.hpp"
#include "MandelSet.h"
#include "utils_main.h"


int MandelSettingsWindow(
	sf::RenderWindow *parent, 
	MandelSet        *mandel, 
	bool             *close_cond,
	sf::Font         &global_font);