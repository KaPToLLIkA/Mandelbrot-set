#pragma once
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/Text.hpp"
#include "imgui.h"
#include "imgui-SFML.h"

enum FontT {
	small,
	middle,
	big,
	bigbig
};


sf::Text& showBoldText20(std::string str, sf::Vector2f pos, sf::Font &f);
sf::Text& showBoldText20(const std::string *str, sf::Vector2f pos, sf::Font &f);

inline void LoadCustomFont()
{
	ImGui::GetIO().Fonts->AddFontFromFileTTF("data\\monof55.ttf", 12.f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	ImGui::GetIO().Fonts->AddFontFromFileTTF("data\\monof55.ttf", 20.f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	ImGui::GetIO().Fonts->AddFontFromFileTTF("data\\monof55.ttf", 28.f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	ImGui::GetIO().Fonts->AddFontFromFileTTF("data\\monof55.ttf", 36.f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	ImGui::SFML::UpdateFontTexture();
}