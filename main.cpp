#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <complex>
#include <chrono>
#include <thread>
//#include <map>
#include <future>

int kWindowsSizeX = 800;
int kWindowsSizeY = 800;

int kResolution = 800;
int kNbThread = 8;
int kDepth = 125;

/*long double k_xmin = -2;
long double k_xmax = 0.5;
long double k_ymin = -1.25;
long double k_ymax = 1.25;*/

long double center_point_x = -1.985540371654130485531439267191269851811165434636382820704394766801377;
long double center_point_y = 0.000000000000000000000000000001565120217211466101983496092509512479178;

long double k_initial_xmin = center_point_x - 2; //0.0013299 //1.3299
long double k_initial_xmax = center_point_x + 2;
long double k_initial_ymin = center_point_y - 2; //0.980002
long double k_initial_ymax = center_point_y + 2;


long double k_xmin = center_point_x - 2; //0.0013299 //1.3299
long double k_xmax = center_point_x + 2;
long double k_ymin = center_point_y - 2; //0.980002
long double k_ymax = center_point_y + 2;

float zoom_factor = 1;


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
			//xmin + ((x / kResolution) * (xmax - xmin)), ymin + ((y / kResolution) * (ymax - ymin)));
			long double delta_x = (x * 1.0 / (window_x_max - window_x_min)) * (xmax - xmin);
			long double delta_y = (y * 1.0 / (window_y_max - window_y_min)) * (ymax - ymin);
			std::complex<long double> coordinate(xmin + delta_x, k_ymin + delta_y);
			int deviation = mandelbrot(coordinate);

			out.insert(std::make_pair(std::make_pair(x, y), deviation));
		}
	}
	return out;
}


sf::Image mandelbrot_set(long double xmin, long double xmax, long double ymin, long double ymax) {
	sf::Image image;
	image.create(kWindowsSizeX, kWindowsSizeY, sf::Color((int) xmax / xmin));
	std::vector<std::future<std::map<std::pair<int, int>, int>>> parts;

	long double delta_x = xmax - xmin;
	long double delta_y = ymax - ymin;

	for (int i = 0; i < kNbThread; ++i) {
		long double a = xmin;// xmin + (delta_x * i / kNbThread);
		long double b = xmax; // a + (delta_x * 1 / kNbThread);
		long double c = ymin + (delta_y * i / kNbThread);
		long double d = ymin + (delta_y * (i + 1.0) / kNbThread);

		int e = 0;//kWindowsSizeX - (((kNbThread - i*1.0) / kNbThread) * kWindowsSizeX);
		int f = kWindowsSizeX;// -(((kNbThread - (i + 1.0)) / kNbThread) * kWindowsSizeX);
		int g = kWindowsSizeY * i / kNbThread;// - (((kNbThread - i) / kNbThread*1.0) * kWindowsSizeY*1.0);
		int h = (kWindowsSizeY * (i + 1.0) / kNbThread);//kWindowsSizeY - (((kNbThread - (i + 1)) / kNbThread) * kWindowsSizeY);


		parts.push_back(std::async(partial_brot, a, b, c, d, e, f, g, h));
	}

	for (auto &part : parts) {
		auto new_stuff = part.get();
		for (auto &point : new_stuff) {
			int x = point.first.first;
			int y = point.first.second;
			int deviation = point.second;
			int color = deviation  * kDepth % 256;//0;
			if (deviation < kDepth) {
				color = 255 * (deviation*1.0 / kDepth*1.0);
				image.setPixel(x, y, sf::Color((deviation * 7)%256, (deviation * 41) % 256, (deviation * 127) % 256));//(1117111 / (deviation+1))); //1117111 is a prime number, I thought I would give a cool effect #TODO: make tunable
			}
		}
	}
	return image;
}

int main() {
	sf::RenderWindow window(sf::VideoMode(kWindowsSizeX, kWindowsSizeY), "fractal_display", sf::Style::Close);
	window.setVerticalSyncEnabled(true);

	sf::Image image;
	sf::Texture texture;
	sf::Sprite sprite;

	bool display_changed = false;
	image = mandelbrot_set(k_xmin, k_xmax, k_ymin, k_ymax);
	image.saveToFile("C:\\Users\\Julien\\Pictures\\mandel.png");

	int frame = 0;
	while (window.isOpen() /*&& frame < 1200*/)
	{
		++frame;
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
					long double delta = (k_xmax - k_xmin) * 0.05;
					center_point_x -= delta;
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
					long double delta = (k_xmax - k_xmin) * 0.05;
					center_point_x += delta;
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
					long double delta = (k_ymax - k_ymin) * 0.05;
					center_point_y -= delta;
					display_changed = true;
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
					long double delta = (k_ymax - k_ymin) * 0.05;
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
		/*zoom_factor *= 1.1;
		std::string path = "C:\\Users\\Julien\\Pictures\\fractal_video\\mandel" + std::to_string(frame) + ".png";
		image.saveToFile(path);
		long double delta_x = std::abs((k_initial_xmax - k_initial_xmin) / zoom_factor);
		long double delta_y = std::abs((k_initial_ymax - k_initial_ymin) / zoom_factor);
		//kDepth += (kDepth / 0.01 == 0 ? 1 : kDepth * 0.01);
		kDepth *= 1.05;
		k_xmin = center_point_x - (2 / zoom_factor);
		k_xmax = center_point_x + (2 / zoom_factor);
		k_ymin = center_point_y - (2 / zoom_factor);
		k_ymax = center_point_y + (2 / zoom_factor);
		display_changed = true;
		*/
		/*END OF VIDEO HACK*/

		if (display_changed) {
			k_xmin = center_point_x - (2 / zoom_factor);
			k_xmax = center_point_x + (2 / zoom_factor);
			k_ymin = center_point_y - (2 / zoom_factor);
			k_ymax = center_point_y + (2 / zoom_factor);
			image = mandelbrot_set(k_xmin, k_xmax, k_ymin, k_ymax);
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