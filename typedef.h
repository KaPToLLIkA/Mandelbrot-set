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

//Graphics element with image
struct GEwithIMG
{
	GEwithIMG(std::string way_to_resource)
	{
		image.loadFromFile(way_to_resource);
		texture.loadFromImage(image);
		sprite.setTexture(texture);
	}
	GEwithIMG() {};

	sf::Texture texture;
	sf::Image	image;
	sf::Sprite	sprite;
};

//Graphics element without image
struct GE
{
	GE(std::string way_to_resource)
	{
		image.loadFromFile(way_to_resource);
		texture.loadFromImage(image);
		sprite.setTexture(texture);
	}
	GE() {};

	sf::Texture texture;
	sf::Image	image;
	sf::Sprite	sprite;
};


