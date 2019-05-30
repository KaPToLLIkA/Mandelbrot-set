#pragma once
#include <mutex>
#include <SFML/Graphics.hpp>

struct Frame
{
	std::mutex	locker;
	sf::Texture texture;
	sf::Image	image;
	sf::Sprite	sprite;
};
