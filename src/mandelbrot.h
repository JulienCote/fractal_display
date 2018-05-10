#pragma once

#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <chrono>
#include <thread>
#include <future>
#include <mutex>


#include "simple_complex.h"
#include "mandelbrot_gpu.cuh"

template<class T>
class Mandelbrot{
    public:
        Mandelbrot(bool gpu_activated = false){
            this->gpu_activated = gpu_activated;
        };

        int mandelbrot(SimpleComplex& value) {
            SimpleComplex base = value;
            for (int i = 0; i < kDepth; ++i) {
                if (4 < value.magnitude_squared()) {
                    return i;
                }
                value *= value;
                value += base;
            }
            return 0;
        }

        void partial_brot(const T& xmin, const  T& xmax, T&& ymin, T&& ymax, int&& window_x_min, const int& window_x_max, int&& window_y_min, int&& window_y_max, sf::Image& image) {
            std::map<std::pair<int, int>, int> out;

            for (int x = window_x_min; x < window_x_max; ++x) {
                for (int y = window_y_min; y < window_y_max; ++y) {
                    T delta_x = (x * 1.0 / (window_x_max - window_x_min)) * (xmax - xmin);
                    T delta_y = (y * 1.0 / (window_y_max - window_y_min)) * (ymax - ymin);
                    SimpleComplex coordinate(xmin + delta_x, k_ymin + delta_y);
                    int deviation = mandelbrot(coordinate);
                    if (deviation < kDepth) {
                        //image.setPixel(x, y, sf::Color((deviation * 7) % 256, (deviation * 41) % 256, (deviation * 127) % 256));
                        image.setPixel(x, y, sf::Color((deviation + 7) % 256, (deviation + 41) % 256, (deviation +127) % 256));
                        // image.setPixel(x, y, sf::Color(0, 0, (deviation) % 255));
                    }
                }
            }
        }


        void mandelbrot_set(const T& xmin, const T& xmax, const T& ymin, const T& ymax, sf::Image& image) {
            std::vector<std::thread> parts;

            T delta_y = ymax - ymin;

            for (int i = 0; i < kNbThread; ++i) {
                T a = ymin + (i * 1.0 / kNbThread * delta_y);
                T b = ymin + ((i + 1.0) / kNbThread * delta_y);

                int c = static_cast<int>(std::round(kWindowsSizeY * i * 1.0 / kNbThread));
                int d = static_cast<int>(std::round(kWindowsSizeY * (i + 1.0) / kNbThread));

                parts.push_back(std::thread(&Mandelbrot::partial_brot, this, std::ref(xmin), std::ref(xmax), std::move(a), std::move(b), 0, std::ref(kWindowsSizeX), std::move(c), std::move(d), std::ref(image)));
            }

            for (auto &t : parts) {
                t.join();
            }
        }
        void render(sf::Image& image) {
            k_xmin = center_point_x - (2 / zoom_factor);
			k_xmax = center_point_x + (2 / zoom_factor);
			k_ymin = center_point_y - (2 / zoom_factor);
			k_ymax = center_point_y + (2 / zoom_factor);

            if (gpu_activated == true) {
                unsigned int* pixels = new unsigned int[kWindowsSizeX * kWindowsSizeY];
                render_gpu(k_xmin, k_xmax, k_ymin, k_ymax, kWindowsSizeX, kWindowsSizeY, pixels, kDepth);

                for (int y = 0; y < kWindowsSizeY; ++y) {
                    for (int x = 0; x < kWindowsSizeX; ++x) {
                        int deviation = pixels[(y*kWindowsSizeX) + x];
                        if (deviation > 0) {
                            image.setPixel(x, y, sf::Color((deviation + 7) % 255, (deviation + 41) % 255, (deviation +127) % 255));
                        }
                    }
                }

                delete(pixels);
            } else {
                mandelbrot_set(k_xmin, k_xmax, k_ymin, k_ymax, image);
            }
        }

        void move(float x_factor, float y_factor) {
            if (x_factor > 0) {
                move_right(x_factor/2);
            } else {
                move_left(-1*x_factor/2);
            }

            if (y_factor > 0) {
                move_down(y_factor/2);
            } else {
                move_up(-1*y_factor/2);
            }
        }

        void move_left(float factor){
            T delta = (k_xmax - k_xmin) * factor;
			center_point_x -= delta;
        };

        void move_right(float factor){
            T delta = (k_xmax - k_xmin) * factor;
            center_point_x += delta;
        };
        void move_up(float factor){
            T delta = (k_ymax - k_ymin) * factor;
            center_point_y -= delta;
        };
        void move_down(float factor){
            T delta = (k_ymax - k_ymin) * factor;
            center_point_y += delta;
        };
        void move_in(float factor){
            zoom_factor *= factor;
        };
        void move_out(float factor){
            zoom_factor /= factor;
        };
        void precision_increase(float factor){
            kDepth *= factor;
        };
        void precision_decrease(float factor){
            kDepth /= factor;
        };

        T center_point_x = 0.25467168048098116;
        T center_point_y = 0.00054712897737948770;

        T k_initial_xmin = center_point_x - 2; //0.0013299 //1.3299
        T k_initial_xmax = center_point_x + 2;
        T k_initial_ymin = center_point_y - 2; //0.980002
        T k_initial_ymax = center_point_y + 2;

        T k_xmin = center_point_x - 2; //0.0013299 //1.3299
        T k_xmax = center_point_x + 2;
        T k_ymin = center_point_y - 2; //0.980002
        T k_ymax = center_point_y + 2;

        int kDepth = 100;

        int kWindowsSizeX = 1920;
        int kWindowsSizeY = 1080;

        int kResolution = 800;
        int kNbThread = 8;
        double zoom_factor = 1;

        bool gpu_activated;
};

