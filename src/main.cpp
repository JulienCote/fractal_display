#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>
#include <future>
#include <mutex>

#include <boost/multiprecision/cpp_dec_float.hpp>

#include "simple_complex.h"
#include "mandelbrot.h"

//typedef boost::multiprecision::cpp_dec_float_50 mp_number;
typedef long double mp_number;

int kWindowsSizeX = 1920;
int kWindowsSizeY = 1080;

int kResolution = 800;
int kNbThread = 8;

/*mp_number k_xmin = -2;
mp_number k_xmax = 0.5;
mp_number k_ymin = -1.25;
mp_number k_ymax = 1.25;*/

//mp_number center_point_x = -1.985540371654130485531439267191269851811165434636382820704394766801377; //0.25467168048098116
//mp_number center_point_y = 0.000000000000000000000000000001565120217211466101983496092509512479178; //0.00054712897737948770


int main( int argc, char** argv ) {
	Mandelbrot<mp_number> mandelbrot{true};
	// Mandelbrot mandelbrot{};
	sf::RenderWindow window(sf::VideoMode(kWindowsSizeX, kWindowsSizeY), "fractal_display", sf::Style::Close);
	window.setVerticalSyncEnabled(true);

	sf::Image image;
	sf::Texture texture;
	sf::Sprite sprite;

	image.create(kWindowsSizeX, kWindowsSizeY, sf::Color::White);

	bool display_changed = true;

	// std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	// mandelbrot_set(k_xmin, k_xmax, k_ymin, k_ymax, image);
	// std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

	// auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	// std::cout << duration << '\n';

	//image.saveToFile("C:\\Users\\Julien\\Pictures\\mandel.png");

	//int frame = 0;
	while (window.isOpen()/*&& frame < 3000*/)
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
					mandelbrot.move_left(0.05);
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
					mandelbrot.move_right(0.05);
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
					mandelbrot.move_up(0.05);
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
					mandelbrot.move_down(0.05);
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add)) {
					mandelbrot.move_in(1.1);
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract)) {
					mandelbrot.move_out(1.1);
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1)) {
					mandelbrot.precision_increase(2);
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad4)) {
					mandelbrot.precision_decrease(2);
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad0)) {
					mandelbrot.toggle_rendering_type();
					display_changed = true;
				}
			}
			if(event.type == sf::Event::MouseButtonPressed) {
				if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
					sf::Vector2i position = sf::Mouse::getPosition(window);
					const float middle_x = kWindowsSizeX / 2.0;
					const float middle_y = kWindowsSizeY / 2.0;

					float factor_x = (position.x - middle_x) / middle_x;
					float factor_y = (position.y - middle_y) / middle_y;

					mandelbrot.move(factor_x, factor_y);
					display_changed=true;
				}
			}
		}

		/*VIDEO RECORD HACK */
		/*++frame;
		zoom_factor *= 1.01;
		std::string path = "C:\\Users\\Julien\\Pictures\\fractal_video\\mandel" + std::to_string(frame) + ".png";
		image.saveToFile(path);
		mp_number delta_x = std::abs((k_initial_xmax - k_initial_xmin) / zoom_factor);
		mp_number delta_y = std::abs((k_initial_ymax - k_initial_ymin) / zoom_factor);
		//kDepth += (kDepth / 0.01 == 0 ? 1 : kDepth * 0.01);
		kDepth *= 1.01;
		k_xmin = center_point_x - (2 / zoom_factor);
		k_xmax = center_point_x + (2 / zoom_factor);
		k_ymin = center_point_y - (2 / zoom_factor);
		k_ymax = center_point_y + (2 / zoom_factor);
		display_changed = true;

		/*END OF VIDEO HACK*/

		if (display_changed) {
			image.create(kWindowsSizeX, kWindowsSizeY, sf::Color::White);
			mandelbrot.render(image);
		}

		window.clear();
		texture.loadFromImage(image);
		sprite.setTexture(texture, true);
		window.draw(sprite);
		window.display();
		display_changed = false;

		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	return 0;
}