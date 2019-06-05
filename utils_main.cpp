#include "utils_main.h"

sf::Text & showBoldText20(std::string str, sf::Vector2f pos, sf::Font & f)
{
	static sf::Text text;
	text.setFont(f);
	text.setOrigin(0.0f, 0.0f);
	text.setStyle(sf::Text::Style::Bold);
	text.setFillColor(sf::Color::White);
	text.setCharacterSize(20);
	text.setPosition(pos);
	text.setString(str);
	return text;
}

sf::Text & showBoldText20(const std::string *str, sf::Vector2f pos, sf::Font & f)
{
	static sf::Text text;
	text.setFont(f);
	text.setOrigin(0.0f, 0.0f);
	text.setStyle(sf::Text::Style::Bold);
	text.setFillColor(sf::Color::White);
	text.setCharacterSize(20);
	text.setPosition(pos);
	text.setString(*str);
	return text;
}
