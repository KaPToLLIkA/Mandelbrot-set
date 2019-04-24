# Mandelbrot-set
This is a visualization of the Mandelbrot set.

Each pixel on the screen is a point of the complex plane. It is checked by the algorithm for belonging to the set and painted in a certain color, depending on the belonging to the set.

![image](https://dik.academic.ru/pictures/wiki/files/77/Mandel_zoom_07_satellite.jpg "Множество Мандельброта")


# Demonstration of work 

empty, will added later


# How to build a project

1) Download ImGui [here](https://github.com/coconut/imgui)


2) Download SFML [here](https://www.sfml-dev.org/download.php)

3) Download ImGui binding for SFML [here](https://github.com/eliasdaler/imgui-sfml)

4) Study [the article](https://habr.com/ru/post/335512/), there is a step-by-step instruction

5) Study [the article](http://kychka-pc.ru/sfml/urok-1-podklyuchenie-biblioteki-k-srede-razrabotki-visual-studio-2013.html) to connect to the SFML project

6) Study [the article](http://kuchka-pc.ru/wiki/svobodnaya-baza-znanij-html/uroki-sfml/staticheskaya-linkovka-sfml-visual-studio-2015) for static linking SFML to the project

7) Set up your project for x32 platform (use this preprocessor defenitions: _CRT_SECURE_NO_WARNINGS and _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)