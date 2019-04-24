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
#include "mandelbrot_set.h"
#include "portable-file-dialogs.h"

#define IM_X 800
#define IM_Y 800
#define THREADS_COUNT 2









namespace params {

	
	sf::Vector2u	main_window_size = { IM_X, IM_Y },
					img_size = { IM_X, IM_Y },
					settings_window_size = {400, IM_Y};

	sf::Vector2f	img_pos = { float(img_size.x >> 1), float(img_size.y >> 1) };
												
	double		scale = 1.0f,
				radius = 8.0f,
				delta_mx = 0.0f,
				delta_my = 0.0f,
				mx = Mandelbrot::countMx_My(0, main_window_size.x - 1, scale, delta_mx),
				my = Mandelbrot::countMx_My(0, main_window_size.y - 1, scale, delta_my);


	unsigned	number_of_color_points = 4,
				number_of_iterations = number_of_color_points * 12,
				number_of_render_threads = THREADS_COUNT,
				number_of_saved_img = 0,
				next_x = 0, 
				next_y = 0;

	bool settings_window_closed = true;
	bool main_window_closed = false;
	bool color_change = false;
	bool frame_saved = false;
	bool windows_magnet = false;
	

	std::vector<std::thread> threads;
	std::thread *settings_thread;
}











Mandelbrot::MandelbrotSet set(params::img_size, params::img_pos, params::number_of_render_threads, params::number_of_color_points);
sf::RenderWindow main_window(sf::VideoMode(params::main_window_size.x, params::main_window_size.y), " ", sf::Style::Default);
sf::RenderWindow settings_window;



sf::Text iter_count;
sf::Text scale;
sf::Text info;
sf::Text info1;
sf::Text info2;
sf::Font font;



void updateScaleText();
void updateIterationsText();
void showSettings();
void updateSprite();
void drawColorPicker();
void saveFrame();
void renderThread(unsigned id);


std::string utf8_to_string(const char *utf8str, const std::locale& loc);


int main() 
{
	assert (!(params::number_of_color_points < 4) && "NUMBER OF COLOR POINTS SHOULD BE GREATER THAN 3");

	
	main_window.setVerticalSyncEnabled(true);
	main_window.setFramerateLimit(60);
	


	set.createColorsTable(params::number_of_iterations);

	for (size_t i = 0; i < THREADS_COUNT; ++i)
	{
		try
		{
			params::threads.push_back(std::thread(renderThread, i));
			params::threads[i].detach();
		}
		catch (std::system_error error)
		{
			std::cout << error.code();
		}
	}
	
	
	



	
	font.loadFromFile("12144.ttf");
	iter_count.setFont(font);
	iter_count.setOrigin(0.0f, 0.0f);
	iter_count.setStyle(sf::Text::Style::Bold);
	iter_count.setFillColor(sf::Color::White);
	iter_count.setPosition(5.0f, 5.0f);
	iter_count.setCharacterSize(20);

	scale.setFont(font);
	scale.setOrigin(0.0f, 0.0f);
	scale.setStyle(sf::Text::Style::Bold);
	scale.setFillColor(sf::Color::White);
	scale.setPosition(250.0f, 5.0f);
	scale.setCharacterSize(20);

	
	info.setFont(font);
	info.setOrigin(0.0f, 0.0f);
	info.setStyle(sf::Text::Style::Bold);
	info.setFillColor(sf::Color::White);
	info.setPosition(5.0f, IM_Y - 50.f);
	info.setCharacterSize(20);
	info.setString("S - save frame to jpg\n"
				   "Arrows up and down - Detalization");


	info1.setFont(font);
	info1.setOrigin(0.0f, 0.0f);
	info1.setStyle(sf::Text::Style::Bold);
	info1.setFillColor(sf::Color::White);
	info1.setPosition(400.0f, IM_Y - 50.f);
	info1.setCharacterSize(20);
	info1.setString("Shift + C - open color picker\n"
					"Mouse - navigation");


	info2.setFont(font);
	info2.setOrigin(0.0f, 0.0f);
	info2.setStyle(sf::Text::Style::Bold);
	info2.setFillColor(sf::Color::Red);
	info2.setPosition(5.0f, IM_Y - 25.f);
	info2.setCharacterSize(20);
	info2.setString("Navigation disabled, close color picker");

	updateIterationsText();
	updateScaleText();


	

	sf::Image center_img;
	center_img.loadFromFile("center.png");
	sf::Texture center_tex;
	center_tex.loadFromImage(center_img);
	sf::Sprite center_sp;
	center_sp.setTexture(center_tex);
	center_sp.setOrigin(20.0f, 20.0f);
	center_sp.scale(0.4f, 0.4f);
	center_sp.setPosition(IM_X / 2.0f, IM_Y / 2.0f);



	sf::Vector2i last_mouse_pos = { IM_X / 2, IM_Y / 2 };
	
	bool mouse_button_pressed = false;
	


	
	sf::Clock deltaClock;
	sf::Clock deltaRender;
	deltaRender.restart();
	while (main_window.isOpen()) 
	{
		sf::Vector2i current_mouse_pos = last_mouse_pos;
		
		

		sf::Event events_handler;
		if (params::color_change)
		{
			while (main_window.pollEvent(events_handler))
			{
				if (events_handler.type == sf::Event::Resized) main_window.setSize(params::main_window_size);
				if (events_handler.type == sf::Event::Closed) params::settings_window_closed = true;
				
			}
		}
		else
		{

			current_mouse_pos = sf::Mouse::getPosition();







			while (main_window.pollEvent(events_handler))
			{


				if (events_handler.type == sf::Event::Closed) params::main_window_closed = true;
				if (events_handler.type == sf::Event::Resized) main_window.setSize(params::main_window_size);


				if (events_handler.type == sf::Event::MouseWheelMoved)
				{

					if (events_handler.mouseWheel.delta == -1)
					{
						
						params::scale *= 1.2f;
						updateScaleText();
						set.update_sprite = true;
						set.rendered = false;
					}
					else if (events_handler.mouseWheel.delta == 1)
					{

						
						params::scale /= 1.2f;
						updateScaleText();
						set.update_sprite = true;
						set.rendered = false;


					}

				}
				if (events_handler.type == sf::Event::MouseButtonPressed)
				{

					mouse_button_pressed = true;
					last_mouse_pos = current_mouse_pos;


				}
				if (events_handler.type == sf::Event::MouseButtonReleased)
				{

					mouse_button_pressed = false;
					set.update_sprite = true;
					set.rendered = false;

				}

			}
		}


		static auto c1 = set.img.getPixel(0,        0);
		static auto c2 = set.img.getPixel(IM_X - 1, 0);
		static auto c3 = set.img.getPixel(0,        IM_Y - 1);
		static auto c4 = set.img.getPixel(IM_X - 1, IM_Y - 1);



		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && sf::Keyboard::isKeyPressed(sf::Keyboard::C))
		{
			if (params::settings_window_closed)
			{
				if(mouse_button_pressed)
					mouse_button_pressed = false;
				params::settings_window_closed = false;
				params::settings_thread = new std::thread(showSettings);
				params::settings_thread->detach();
			}

		}
		






		if (set.update_sprite && deltaRender.getElapsedTime().asMilliseconds() > 500)
		{
			deltaRender.restart();
			updateSprite();
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !set.update_sprite) 
		{
			params::number_of_iterations += 5;
			
			if (mouse_button_pressed)
				mouse_button_pressed = false;
			set.update_colors = true;
			set.update_sprite = true; 
			set.rendered = false;
			
			updateIterationsText();
		}









		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && !set.update_sprite) 
		{
			if (!(params::number_of_iterations < params::number_of_color_points))
				params::number_of_iterations -= 5;

			
			if(mouse_button_pressed)
				mouse_button_pressed = false;
			set.update_colors = true;
			set.update_sprite = true;
			set.rendered = false;

			updateIterationsText();
		}













		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && !params::frame_saved) 
		{
			params::frame_saved = true;
			saveFrame();
		}
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::S)) params::frame_saved = false;



		if (mouse_button_pressed) 
		{
			params::delta_mx += Mandelbrot::countDeltaMx_My(last_mouse_pos.x, current_mouse_pos.x, params::scale);
			params::delta_my += Mandelbrot::countDeltaMx_My(last_mouse_pos.y, current_mouse_pos.y, params::scale);
			set.sp.setPosition(set.sp.getPosition().x + (last_mouse_pos.x - current_mouse_pos.x) * -1,
								set.sp.getPosition().y + (last_mouse_pos.y - current_mouse_pos.y) * -1);

		}
		else
		{
			set.lock_img.lock();
			set.tex.loadFromImage(set.img);
			set.sp.setTexture(set.tex);
			set.sp.setPosition(params::img_pos);
			set.lock_img.unlock();
		}
		last_mouse_pos = current_mouse_pos;



		
		

		main_window.clear({	sf::Uint8((c1.r + c2.r + c3.r + c4.r) / 4),
							sf::Uint8((c1.g + c2.g + c3.g + c4.g) / 4),
							sf::Uint8((c1.b + c2.b + c3.b + c4.b) / 4),
							sf::Uint8((c1.a + c2.a + c3.a + c4.a) / 4) });
		
		main_window.draw(set.sp);
		main_window.draw(iter_count);
		main_window.draw(scale);
		if (params::color_change)
		{
			main_window.draw(info2);
		}
		else
		{
			main_window.draw(info);
			main_window.draw(info1);
		}
		main_window.draw(center_sp);
		

		
		main_window.display();

		if (params::main_window_closed) 
		{
			//saving settings
			
			main_window.close();
		}
	

	}
	set.main_window_closed = true;
	set.restartRender();
	
	
	

	return 0;
}


void updateScaleText()
{
	static std::string buffer;
	static std::stringstream converter;
	converter << params::scale;
	converter >> buffer;
	converter.clear();

	scale.setString("Zoom: " + buffer);
}

void updateIterationsText()
{
	static std::string buffer;
	static std::stringstream converter;
	converter << params::number_of_iterations;
	converter >> buffer;
	converter.clear();

	iter_count.setString("Detalization: " + buffer);
}

void showSettings()
{
	settings_window.create(sf::VideoMode(params::settings_window_size.x, params::settings_window_size.y), " ", sf::Style::Default);
	settings_window.setFramerateLimit(60);
	params::color_change = true;
	ImGui::SFML::Init(settings_window, true);
	sf::Clock deltaClock;
	while (settings_window.isOpen()) {
		if(params::windows_magnet)
			settings_window.setPosition({ main_window.getPosition().x - 400,main_window.getPosition().y });

		sf::Event events_handler;
		while (settings_window.pollEvent(events_handler))
		{
			ImGui::SFML::ProcessEvent(events_handler);
			if (events_handler.type == sf::Event::Closed) params::settings_window_closed = true;
			if (events_handler.type == sf::Event::Resized) settings_window.setSize(params::settings_window_size);
		}
		ImGui::SFML::Update(settings_window, deltaClock.restart());
		settings_window.clear({ 0, 0, 0, 255 });


		drawColorPicker();


		ImGui::SFML::Render(settings_window);
		settings_window.display();
		if (params::settings_window_closed
			|| params::main_window_closed)
		{

			settings_window.close();
		}
	}
	params::color_change = false;
	delete params::settings_thread;
}







void updateSprite()
{
	set.stop_render = true;
	params::next_x = 0;
	params::next_y = 0;
	params::mx = Mandelbrot::countMx_My(0, IM_X - 1, params::scale, params::delta_mx);
	params::my = Mandelbrot::countMx_My(0, IM_Y - 1, params::scale, params::delta_my);

	if (set.update_colors)
	{
		set.createColorsTable(params::number_of_iterations);
		
	}
	set.restartRender();


	
	set.stop_render = false;
	set.update_sprite = false;
	
	
}

static bool enable_auto_update = true;
static bool cleaned = false;
static bool print_success_msg = false;
static bool iter_changed = false;
static std::array <char, 64> filename;

void drawColorPicker()
{
	

	ImGui::SetNextWindowPos({0, 0});
	ImGui::Begin("Color Picker", nullptr, params::settings_window_size, 1.0f,
		ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_::ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_::ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_::ImGuiWindowFlags_NoSavedSettings);
	
	ImGui::BeginChild("##colors", ImVec2(0.f, params::settings_window_size.y - 300.f), true);
	for (size_t i = 0; i < params::number_of_color_points; ++i)
	{	
		ImGui::PushID(i);
		float colors[4] = {
			float(set.color_point[i].r) / 255.f,
			float(set.color_point[i].g) / 255.f,
			float(set.color_point[i].b) / 255.f,
			float(set.color_point[i].a) / 255.f
		};
		if (ImGui::ColorEdit4("colorPic", colors, 
			ImGuiColorEditFlags_::ImGuiColorEditFlags_NoLabel |
			ImGuiColorEditFlags_::ImGuiColorEditFlags_Float |
			ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar))
		{
			if (enable_auto_update)
			{
				
				set.color_point[i] = sf::Color(
					sf::Uint8(colors[0] * 255.f),
					sf::Uint8(colors[1] * 255.f),
					sf::Uint8(colors[2] * 255.f),
					sf::Uint8(colors[3] * 255.f)
				);
				
				set.stop_render = true;
				set.update_colors = true;
				set.update_pixels = true;
				set.update_sprite = true;
			}
			else
			{
				set.color_point[i] = sf::Color(
					sf::Uint8(colors[0] * 255.f),
					sf::Uint8(colors[1] * 255.f),
					sf::Uint8(colors[2] * 255.f),
					sf::Uint8(colors[3] * 255.f)
				);

			}
		}
		ImGui::SameLine();
		ImGui::SetCursorPosX(400.f - ImGui::GetStyle().ItemSpacing.x * 3.f - 20.f);
		if (ImGui::Button("X", { 20.f, 20.f }))
		{
			if (params::number_of_color_points > 1)
			{
				--params::number_of_color_points;
				set.color_point.erase(set.color_point.begin() + i);
				set.stop_render = true;
				set.update_colors = true;
				set.update_pixels = true;
				set.update_sprite = true;

			}

		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::TextColored({ 1.f, 0.f, 0.f, 1.f }, "Delete color point...");

			ImGui::EndTooltip();

		}
		ImGui::PopID();
	}
	ImGui::EndChild();
	if (ImGui::Button("Add new color", ImVec2(400.f - ImGui::GetStyle().ItemSpacing.x * 2.f, 25.f)))
	{
		set.color_point.push_back({ 0,0,0,255 });
		

		++params::number_of_color_points;
		if (params::number_of_iterations < params::number_of_color_points)
			params::number_of_iterations = params::number_of_color_points;


		updateIterationsText();
		

		set.stop_render = true;
		set.update_colors = true;
		set.update_sprite = true;

	}
	
	if (!cleaned)
	{
		memset(filename.data(), '\0', filename.size());
		cleaned = true;
	}
	
	ImGui::Checkbox("Enable frame auto-updating", &enable_auto_update);
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();

		ImGui::Text("If not active, you will need\n"
					"to manually update the image\n"
					"by clicking on the \"Refresh\n"
					"frame\" button.");

		ImGui::EndTooltip();
	}


	if (ImGui::CollapsingHeader("Save colorset##1"))
	{
		ImGui::InputText("Filename", filename.data(), filename.size());
		if (print_success_msg)
			ImGui::TextColored({ 0.f, 1.f, 0.f, 1.f }, "Saved.");
		else
			ImGui::TextColored({ 1.f, 0.f, 0.f, 1.f }, "Print filename, then press \"Save\".");

		if (ImGui::Button("Save", ImVec2(400.f - ImGui::GetStyle().ItemSpacing.x * 2.f, 25.f)))
		{
			std::string name = utf8_to_string(filename.data(), std::locale(".1251"));
			std::cout << name << std::endl;
			if (!name.empty())
			{
				set.saveColors(name);
				print_success_msg = true;
				cleaned = false;
			}
			else
				print_success_msg = false;

		}
		
	}
	else
	{
		ImGui::NewLine();
		ImGui::Checkbox("Enable magnet for windows", &params::windows_magnet);
		ImGui::Separator();
		ImGui::TextColored({ 0.f, 0.75f, 1.f, 1.f }, "Detalization");
		static float width = (400.f - ImGui::GetStyle().ItemSpacing.x * 5.f) / 4.f;
		if (ImGui::Button("+10", { width, 25.f }))
		{
			params::number_of_iterations += 10;
			if (enable_auto_update)
			{
				set.stop_render = true;
				set.update_colors = true;
				set.update_sprite = true;
				set.rendered = false;
			}
			iter_changed = true;
			updateIterationsText();

		}
		ImGui::SameLine();
		if (ImGui::Button("+1", { width, 25.f }))
		{
			++params::number_of_iterations;

			if (enable_auto_update)
			{
				set.stop_render = true;
				set.update_colors = true;
				set.update_sprite = true;
				set.rendered = false;
			}
			iter_changed = true;


			updateIterationsText();

		}
		ImGui::SameLine();
		if (ImGui::Button("-1", { width, 25.f }))
		{
			--params::number_of_iterations;
			if (params::number_of_iterations < params::number_of_color_points)
				params::number_of_iterations = params::number_of_color_points;



			if (enable_auto_update)
			{
				set.stop_render = true;
				set.update_colors = true;
				set.update_sprite = true;
				set.rendered = false;
			}
			iter_changed = true;

			updateIterationsText();

		}
		ImGui::SameLine();
		if (ImGui::Button("-10", { width, 25.f }))
		{
			params::number_of_iterations -= 10;
			if (params::number_of_iterations < params::number_of_color_points)
				params::number_of_iterations = params::number_of_color_points;



			if (enable_auto_update)
			{
				set.stop_render = true;
				set.update_colors = true;
				set.update_sprite = true;
				set.rendered = false;
			}
			iter_changed = true;


			updateIterationsText();

		}
	}
	ImGui::SetCursorPosY(IM_X - ImGui::GetStyle().ItemSpacing.y * 3.f - 50.f);
	if (ImGui::Button("Refresh frame", ImVec2(400.f - ImGui::GetStyle().ItemSpacing.x * 2.f, 25.f)))
	{
		set.stop_render = true;
		set.update_colors = true;
		set.update_sprite = true;
		if (iter_changed)
		{
			set.rendered = false;
			iter_changed = false;
		}
		else
			set.update_pixels = true;
	}
	if (ImGui::Button("Open saved colorset...", ImVec2(400.f - ImGui::GetStyle().ItemSpacing.x * 2.f, 25.f)))
	{
		auto file = pfd::open_file("Open colors set",
			std::filesystem::current_path().string() + "\\colors\\",
			{ "Saves (.colset)"
			,"*.colset" });
		if (!file.result().empty())
		{
			set.loadColors(*file.result().begin());
			params::number_of_color_points = set.color_point.size();
			if (params::number_of_iterations < params::number_of_color_points)
				params::number_of_iterations = params::number_of_color_points;
			set.stop_render = true;
			set.update_colors = true;
			set.update_sprite = true;


			updateIterationsText();
		}
	}

	ImGui::End();
}









void saveFrame()
{
	std::time_t cur_time = std::time(nullptr);
	std::string time_str = std::ctime(&cur_time);
	
	for (size_t i = 0; i < time_str.size(); ++i)
		if (time_str[i] == ':') time_str[i] = '.';

	time_str.erase(--time_str.end());
	set.lock_img.lock();
	set.img.saveToFile(".\\frames\\" + time_str + ".jpg");
	set.lock_img.unlock();
}




std::string utf8_to_string(const char *utf8str, const std::locale& loc)
{
	// UTF-8 to wstring
	std::wstring_convert<std::codecvt_utf8<wchar_t>> wconv;
	std::wstring wstr = wconv.from_bytes(utf8str);
	// wstring to string
	std::vector<char> buf(wstr.size());
	std::use_facet<std::ctype<wchar_t>>(loc).narrow(wstr.data(), wstr.data() + wstr.size(), '?', buf.data());
	return std::string(buf.data(), buf.size());
}




void renderThread(unsigned id)
{
	
	while (main_window.isOpen())
	{
		set.lock_stream_is_running[id].lock();
		if (set.update_pixels)
		{
			set.updatePixels(params::next_x,
				params::next_y,
				params::img_size);
			set.update_pixels = false;
		}
		if (!set.rendered)
		{
			set.createImgPart(id,
				params::next_x,
				params::next_y,
				params::img_size,
				params::scale,
				params::mx,
				params::my,
				params::number_of_iterations,
				params::radius);
			if (set.stop_render)
				set.rendered = false;
			else
				set.rendered = true;
		}
		set.lock_stream_is_running[id].unlock();
		std::unique_lock <std::mutex> locker(set.lock_render);

		while (!set.restart_render[id])
		{
			set.lock_render_condition.wait(locker);
		}
		set.restart_render[id] = false;
	}
}