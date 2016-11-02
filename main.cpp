#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <complex>
#include <thread>
#include <map>
#include <future>

int kWindowsSizeX = 800;
int kWindowsSizeY = 800;

int kResolution = 800;
int kNbThread = 16;
int kDepth = 8;

long double k_xmin = -2;
long double k_xmax = 0.5;
long double k_ymin = -1.25;
long double k_ymax = 1.25;

int mandelbrot(std::complex<long double> value) {
	std::complex<long double> base = value;
	for (int i = 0; i < kDepth; ++i) {
		if (std::abs(value) > 2) {
			return i;
		}
		value = (value * value) + base;
	}
	return kDepth;
}

std::map<std::pair<int, int>, int> partial_brot(long double xmin, long double xmax, long double ymin, long double ymax, int window_x_min, int window_x_max, int window_y_min, int window_y_max) {
	std::map<std::pair<int, int>, int> out;

	for (int x = window_x_min; x < window_x_max; ++x) {
		for (int y = window_y_min; y < window_y_max; ++y) {
			std::complex<long double> coordinate(xmin + ((x*1.0 / (window_x_max - window_x_min)) * (xmax - xmin)), ymin + ((y*1.0 / (window_y_max - window_y_min)) * (ymax - ymin)));
			int deviation = mandelbrot(coordinate);

			out.insert(std::make_pair(std::make_pair(x, y), deviation));
		}
	}
	return out;
}


sf::Image mandelbrot_set(long double xmin, long double xmax, long double ymin, long double ymax) {
	sf::Image image;
	image.create(kWindowsSizeX, kWindowsSizeY, sf::Color::Blue);
	std::vector<std::future<std::map<std::pair<int, int>, int>>> parts;

	long double delta_x = xmax - xmin;
	long double delta_y = ymax - ymin;

	for (int i = 0; i < kNbThread; ++i) {
		long double a = 0;// xmin + (delta_x * i / kNbThread);
		long double b = xmax; // a + (delta_x * 1 / kNbThread);
		long double c = ymin + (delta_y * i*1.0 / kNbThread*1.0);
		long double d = c + (delta_y * (i*1.0 + 1) / kNbThread*1.0);

		int e = 0;//kWindowsSizeX - (((kNbThread - i*1.0) / kNbThread) * kWindowsSizeX);
		int f = kWindowsSizeX;// -(((kNbThread - (i + 1.0)) / kNbThread) * kWindowsSizeX);
		int g = kWindowsSizeY - (((kNbThread*1.0 - i*1.0) / kNbThread*1.0) * kWindowsSizeY*1.0);
		int h = kWindowsSizeY - (((kNbThread*1.0 - (i + 1.0)) / kNbThread*1.0) * kWindowsSizeY*1.0);


		parts.push_back(std::async(partial_brot, a, b, c, d, e, f, g, h));
	}

	for (auto &part : parts) {
		auto new_stuff = part.get();
		for (auto &point : new_stuff) {
			int x = point.first.first;
			int y = point.first.second;
			int deviation = point.second;
			int color = 0;
			if (deviation < kDepth) {
				color = 255 * (deviation*1.0 / kDepth*1.0);
				image.setPixel(x, y, sf::Color(color, color, color));
			}

			//image.setPixel(, point.first.second, sf::Color(point.second));
		}
	}
	return image;
	/*

	std::map<std::pair<long double, long double>, int> full_brot = parts.back().get();


	for (long double x = 0; x < kResolution; x+=1) {
	for (long double y = 0; y < kResolution; y+=1) {
	std::complex<long double> coordinate(xmin + ((x / kResolution) * (xmax - xmin)), ymin + ((y / kResolution) * (ymax - ymin)));
	int deviation = mandelbrot(coordinate);

	int color = 0;
	if (deviation < kDepth) {
	color = 255 * (deviation*1.0 / kDepth*1.0);
	image.setPixel(x, y, sf::Color(color, color, color));
	}
	}
	}
	return image;*/
}

int main() {
	sf::RenderWindow window(sf::VideoMode(kWindowsSizeX, kWindowsSizeY), "fractal_display", sf::Style::Close);

	sf::Image image;
	sf::Texture texture;
	sf::Sprite sprite;

	bool display_changed = false;
	image = mandelbrot_set(k_xmin, k_xmax, k_ymin, k_ymax);
	image.saveToFile("C:\\Users\\Julien\\Pictures\\mandel.png");

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
					long double delta = (k_xmax - k_xmin) * 0.05;
					k_xmin -= delta;
					k_xmax -= delta;
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
					long double delta = (k_xmax - k_xmin) * 0.05;
					k_xmin += delta;
					k_xmax += delta;
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
					long double delta = (k_ymax - k_ymin) * 0.05;
					k_ymin += delta;
					k_ymax += delta;
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
					long double delta = (k_ymax - k_ymin) * 0.05;
					k_ymin -= delta;
					k_ymax -= delta;
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add)) {
					long double delta_x = (k_xmax - k_xmin) * 0.05;
					long double delta_y = (k_ymax - k_ymin) * 0.05;
					k_xmin += delta_x;
					k_xmax -= delta_x;
					k_ymin += delta_y;
					k_ymax -= delta_y;
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract)) {
					long double delta_x = (k_xmax - k_xmin) * 0.05;
					long double delta_y = (k_ymax - k_ymin) * 0.05;
					k_xmin -= delta_x;
					k_xmax += delta_x;
					k_ymin -= delta_y;
					k_ymax += delta_y;
					display_changed = true;
				}


			}
		}

		if (display_changed) {
			image = mandelbrot_set(k_xmin, k_xmax, k_ymin, k_ymax);; //TODO: make these tunable
		}

		window.clear();
		texture.loadFromImage(image);
		sprite.setTexture(texture, true);
		window.draw(sprite);
		window.display();
		display_changed = false;

	}
	return 0;
}