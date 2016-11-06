#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>
#include <future>
#include <mutex>
#include <array>
#include <algorithm>
#include <iterator>
#include <array>

#include <boost/multiprecision/cpp_dec_float.hpp>

#include "simple_complex.h"

//typedef boost::multiprecision::cpp_dec_float_50 mp_number;
typedef long double mp_number;

int kWindowsSizeX = 800;
int kWindowsSizeY = 800;

int kResolution = 800;
int kNbThread = 8;
int kDepth = 20; //76287

				 /*mp_number k_xmin = -2;
				 mp_number k_xmax = 0.5;
				 mp_number k_ymin = -1.25;
				 mp_number k_ymax = 1.25;*/

				 //mp_number center_point_x = -1.985540371654130485531439267191269851811165434636382820704394766801377; //0.25467168048098116
				 //mp_number center_point_y = 0.000000000000000000000000000001565120217211466101983496092509512479178; //0.00054712897737948770

mp_number center_point_x = 0.25467168048098116;
mp_number center_point_y = 0.00054712897737948770;

mp_number k_initial_xmin = center_point_x - 2; //0.0013299 //1.3299
mp_number k_initial_xmax = center_point_x + 2;
mp_number k_initial_ymin = center_point_y - 2; //0.980002
mp_number k_initial_ymax = center_point_y + 2;

mp_number k_xmin = center_point_x - 2; //0.0013299 //1.3299
mp_number k_xmax = center_point_x + 2;
mp_number k_ymin = center_point_y - 2; //0.980002
mp_number k_ymax = center_point_y + 2;

mp_number zoom_factor = 1; //1344.8523885103336

int mandelbrot(SimpleComplex&& value) {
	SimpleComplex base = value;
	for (int i = 0; i < kDepth; ++i) {
		if (4 < value.magnitude_squared()) {
			return i;
		}
		value *= value;
		value += base;
	}
	return kDepth;
}

void partial_brot(sf::Image& image, std::vector<std::pair<int,int>>::iterator& first_pixel, const std::vector<std::pair<int,int>>::const_iterator& last_pixel) {
	for (; first_pixel < last_pixel; ++first_pixel) {
		const int x = first_pixel->first;
		const int y = first_pixel->second;
		const mp_number delta_x = k_xmin + (x * 1.0 / (kWindowsSizeX) * (k_xmax - k_xmin));
		const mp_number delta_y = k_ymin + (y * 1.0 / (kWindowsSizeY) * (k_ymax - k_ymin));
		const int deviation = mandelbrot(SimpleComplex(delta_x, delta_y));
		if (deviation < kDepth) {
			image.setPixel(std::move(x), std::move(y), sf::Color((deviation * 7) % 256, (deviation * 41) % 256, (deviation * 127) % 256));
		}
	}
}


void mandelbrot_set(sf::Image& image, std::vector<std::pair<int,int>>& coordinates) {
	std::vector<std::thread> parts;

	for (int i = 0; i < kNbThread; ++i) {

		int coords_low = coordinates.size() * i / kNbThread;
		int coords_high = coordinates.size() * (i + 1.0) / kNbThread;
		auto begin = coordinates.begin() + coords_low;
		auto end = coordinates.begin() + coords_high;

		parts.push_back(std::thread(partial_brot, std::ref(image), std::move(begin), std::move(end)));
	}

	for (auto &t : parts) {
		t.join();
	}
}

int main() {
	sf::RenderWindow window(sf::VideoMode(kWindowsSizeX, kWindowsSizeY), "fractal_display", sf::Style::Close);
	window.setVerticalSyncEnabled(true);

	sf::Image image;
	sf::Texture texture;
	sf::Sprite sprite;

	image.create(kWindowsSizeX, kWindowsSizeY, sf::Color::White);

	std::vector<std::pair<int,int>> coordinates;
	coordinates.reserve(kWindowsSizeX * kWindowsSizeY);

	for (int x = 0; x < kWindowsSizeX; ++x) {
		for (int y = 0; y < kWindowsSizeY; ++y) {
			coordinates.emplace_back(std::make_pair(x,y));
		}
	}
	std::random_shuffle(coordinates.begin(), coordinates.end());
	bool display_changed = true;

	int frame = 0; //724
	while (window.isOpen() && frame < 3000)
	{
		++frame;
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
					mp_number delta = (k_xmax - k_xmin) * 0.05;
					center_point_x -= delta;
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
					mp_number delta = (k_xmax - k_xmin) * 0.05;
					center_point_x += delta;
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
					mp_number delta = (k_ymax - k_ymin) * 0.05;
					center_point_y -= delta;
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
					mp_number delta = (k_ymax - k_ymin) * 0.05;
					center_point_y += delta;
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add)) {
					zoom_factor *= 1.1;
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract)) {
					zoom_factor /= 1.1;
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1)) {
					kDepth *= 2;
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad4)) {
					kDepth /= 2;
					display_changed = true;
				}
			}
		}

		/*VIDEO RECORD HACK */
		/*zoom_factor *= 1.01;
		std::string path = "C:\\Users\\Julien\\Pictures\\fractal_video\\mandel" + std::to_string(frame) + ".png";
		image.saveToFile(path);
		kDepth *= 1.01;
		display_changed = true;
		/*END OF VIDEO HACK*/

		if (display_changed) {
			k_xmin = center_point_x - (2 / zoom_factor);
			k_xmax = center_point_x + (2 / zoom_factor);
			k_ymin = center_point_y - (2 / zoom_factor);
			k_ymax = center_point_y + (2 / zoom_factor);
			image.create(kWindowsSizeX, kWindowsSizeY, sf::Color::White);

			std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
			mandelbrot_set(image, coordinates);
			std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

			std::cout << duration << '\n';
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