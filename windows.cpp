#include "windows.h"

#define MANDEL_SETTINGS_WIDTH 450
int MandelSettingsWindow(
	sf::RenderWindow * parent, 
	MandelSet        * mandel, 
	bool             * close_cond,
	sf::Font         & global_font)
{
	bool start_thread = false;
	std::mutex *frame_locker = new std::mutex;

	sf::RenderWindow window(sf::VideoMode({ MANDEL_SETTINGS_WIDTH, FRAME_MANDEL_Y}), " ", sf::Style::None);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(40);
	ImGui::SFML::Init(window, false);
	LoadCustomFont();


	Range<double>   scale_ = { 2.f, 0.00000001f };
	Range<int>      iter   = { 16, 4096 };
	int number_of_frames = 0;
	sf::Vector2i    frame_size = {FRAME_MANDEL_X, FRAME_MANDEL_Y};

	sf::Clock deltaClock;
	while (parent->isOpen())
	{
		frame_locker->lock();
		window.setPosition(
			{
				parent->getPosition().x - MANDEL_SETTINGS_WIDTH,
				parent->getPosition().y
			}
		);



		sf::Event events_handler;

		while (window.pollEvent(events_handler))
		{
			ImGui::SFML::ProcessEvent(events_handler);
			if (events_handler.type == sf::Event::Resized)  
				window.setSize({ MANDEL_SETTINGS_WIDTH, FRAME_MANDEL_Y });

		}
		ImGui::SFML::Update(window, deltaClock.restart());
		//menu section
		{
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[FontT::middle]);
			ImGui::SetNextWindowPos({ 0.0f, 0.0f }, ImGuiCond_::ImGuiCond_FirstUseEver);
			ImGui::Begin("Settings##MANDEL", nullptr, { MANDEL_SETTINGS_WIDTH, FRAME_MANDEL_Y}, 0.0f,
				ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground |
				ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration |
				ImGuiWindowFlags_::ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_::ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_::ImGuiWindowFlags_NoSavedSettings);

			ImGui::BeginChild("SettingsChild", { 0, FRAME_MANDEL_Y - 60 }, true);
			if (ImGui::CollapsingHeader(u8"Запуск визуализации"))
			{
				ImGui::InputDouble("scale  min", &scale_.min, 0.00000000001f, 0.001f, "%.20f");
				ImGui::InputDouble("scale  max", &scale_.max, 0.00000000001f, 0.001f, "%.20f");
				ImGui::Separator();
				ImGui::NewLine();


				ImGui::InputInt("iter   min", &iter.min, 1, 10);
				ImGui::InputInt("iter   max", &iter.max, 1, 10);
				ImGui::Separator();
				ImGui::NewLine();


				ImGui::Text(u8"Кол-во кадров:");
				ImGui::InputInt("frames num", &number_of_frames, 1, 10);
				ImGui::Separator();
				ImGui::NewLine();

				ImGui::Text(u8"Размер каждого кадра:");
				ImGui::InputInt("size x", &frame_size.x, 1, 10);
				ImGui::InputInt("size y", &frame_size.y, 1, 10);


				float delta = ImGui::GetStyle().ItemSpacing.x;
				if (ImGui::Button(u8"Начать",
					{
						(MANDEL_SETTINGS_WIDTH - delta) / 2.f - delta * 2.f, 50
					}
				))
				{
					start_thread = true;
				}
				ImGui::SameLine();
				if (ImGui::Button(u8"Прервать",
					{
						(MANDEL_SETTINGS_WIDTH - delta) / 2.f - delta * 2.f, 50
					}
				))
				{
					mandel->calculating_funct_started = false;

				}

			}
			ImGui::EndChild();
			if (ImGui::Button(u8"Закрыть",
				{
					MANDEL_SETTINGS_WIDTH - ImGui::GetStyle().ItemSpacing.x * 2.f,
					60.f - ImGui::GetStyle().ItemSpacing.y * 4.f
				}
			)) *close_cond = false;



			ImGui::End();
			ImGui::PopFont();
		}
		//end menu section
		window.clear();



		ImGui::SFML::Render(window);
		window.display();
		if (start_thread && !mandel->calculating_funct_started)
		{
			
			Range<double> sc = { (double)scale_.min, (double)scale_.max };
			Range<unsigned> it = { (unsigned)iter.min, (unsigned)iter.max };
			sf::Vector2u sz = { (unsigned)frame_size.x, (unsigned)frame_size.y };

			std::thread *thr = new std::thread(
				&MandelSet::createFrames,
				mandel,
				sc,
				it,
				sz,
				(unsigned)number_of_frames,
				std::ref(global_font),
				frame_locker
			);
			thr->detach();
			
		}
		
		start_thread = false;

		frame_locker->unlock();
	}
	delete frame_locker;
	return 0;
}
