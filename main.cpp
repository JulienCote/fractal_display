#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <chrono>
#include <thread>
#include <future>
#include <mutex>

#include <boost/multiprecision/cpp_dec_float.hpp>

#include "simple_complex.h"

//typedef boost::multiprecision::cpp_dec_float_50 mp_number;
typedef long double mp_number;

int kWindowsSizeX = 1920;
int kWindowsSizeY = 1080;

int kResolution = 800;
int kNbThread = 8;
int kDepth = 100;

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

double zoom_factor = 1;

int mandelbrot(SimpleComplex& value) {
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

void partial_brot(const mp_number& xmin, const  mp_number& xmax, mp_number&& ymin, mp_number&& ymax, int&& window_x_min, const int& window_x_max, int&& window_y_min, int&& window_y_max, sf::Image& image) {
	std::map<std::pair<int, int>, int> out;

	for (int x = window_x_min; x < window_x_max; ++x) {
		for (int y = window_y_min; y < window_y_max; ++y) {
			mp_number delta_x = (x * 1.0 / (window_x_max - window_x_min)) * (xmax - xmin);
			mp_number delta_y = (y * 1.0 / (window_y_max - window_y_min)) * (ymax - ymin);
			SimpleComplex coordinate(xmin + delta_x, k_ymin + delta_y);
			int deviation = mandelbrot(coordinate);
			if (deviation < kDepth) {
				image.setPixel(x, y, sf::Color((deviation * 7) % 256, (deviation * 41) % 256, (deviation * 127) % 256));
			}
		}
	}
}


void mandelbrot_set(const mp_number& xmin, const mp_number& xmax, const mp_number& ymin, const mp_number& ymax, sf::Image& image) {
	std::vector<std::thread> parts;

	mp_number delta_y = ymax - ymin;

	for (int i = 0; i < kNbThread; ++i) {
		mp_number a = ymin + (i * 1.0 / kNbThread * delta_y);
		mp_number b = ymin + ((i + 1.0) / kNbThread * delta_y);

		int c = static_cast<int>(std::round(kWindowsSizeY * i * 1.0 / kNbThread));
		int d = static_cast<int>(std::round(kWindowsSizeY * (i + 1.0) / kNbThread));

		parts.push_back(std::thread(partial_brot, std::ref(xmin), std::ref(xmax), std::move(a), std::move(b), 0, std::ref(kWindowsSizeX), std::move(c), std::move(d), std::ref(image)));
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

	bool display_changed = false;

	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	mandelbrot_set(k_xmin, k_xmax, k_ymin, k_ymax, image);
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
	std::cout << duration << '\n';

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
			k_xmin = center_point_x - (2 / zoom_factor);
			k_xmax = center_point_x + (2 / zoom_factor);
			k_ymin = center_point_y - (2 / zoom_factor);
			k_ymax = center_point_y + (2 / zoom_factor);
			image.create(kWindowsSizeX, kWindowsSizeY, sf::Color::White);
			mandelbrot_set(k_xmin, k_xmax, k_ymin, k_ymax, image);
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